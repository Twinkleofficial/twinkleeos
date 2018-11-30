/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosio/chain/types.hpp>
#include <eosio/chain/controller.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/block.hpp>
#include <eosio/net_plugin/protocol.hpp>

#include <fc/network/message_buffer.hpp>
#include <fc/network/ip.hpp>
#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/log/appender.hpp>
#include <fc/container/flat.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/crypto/rand.hpp>
#include <fc/exception/exception.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/intrusive/set.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/chain/plugin_interface.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>
#include <regex>
#include <eosio/eoc_relay_plugin/eoc_relay_plugin.hpp>
#include "icp_relay.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#define MAXSENDNUM 100
namespace fc {
   extern std::unordered_map<std::string,logger>& get_logger_map();
}
namespace eosio {

   using fc::time_point;
   using fc::time_point_sec;
   using signed_block_ptr = std::shared_ptr<eosio::chain::signed_block>;
   using eosio::chain_apis::read_only;
   using eosio::chain_apis::read_write;
   using tcp = boost::asio::ip::tcp;
     using boost::asio::ip::tcp;
   using boost::asio::ip::address_v4;
   using boost::asio::ip::host_name;
   using boost::intrusive::rbtree;
   using boost::multi_index_container;

   constexpr auto     def_send_buffer_size_mb = 4;
   constexpr auto     def_send_buffer_size = 1024*1024*def_send_buffer_size_mb;
   constexpr auto     def_max_clients = 25; // 0 for unlimited clients
   constexpr auto     def_max_nodes_per_host = 1;
   constexpr auto     def_conn_retry_wait = 30;
   constexpr auto     def_txn_expire_wait = std::chrono::seconds(3);
   constexpr auto     def_resp_expected_wait = std::chrono::seconds(5);
   constexpr auto     def_sync_fetch_span = 100;
   constexpr uint32_t  def_max_just_send = 1500; // roughly 1 "mtu"
   constexpr bool     large_msg_notify = false;

   constexpr auto     message_header_size = 4;

 
   /**
    *  If there is a change to network protocol or behavior, increment net version to identify
    *  the need for compatibility hooks
    */
   constexpr uint16_t proto_base = 0;
  
    string url = "http://47.105.111.1:9991/";
    string wallet_url = "http://47.105.111.1:55553/";
    eosio::client::http::http_context context;
    bool no_verify = false;
    vector<string> headers;
    auto   tx_expiration = fc::seconds(30);
    const fc::microseconds abi_serializer_max_time = fc::seconds(10); // No risk to client side serialization taking a long time
    string tx_ref_block_num_or_id;
    bool   tx_force_unique = false;
    bool tx_dont_broadcast = false;
    bool tx_return_packed = false;
    bool tx_skip_sign = false;
    bool tx_print_json = false;
    bool print_request = false;
    bool print_response = false;

    uint8_t tx_max_cpu_usage = 0;
    uint32_t tx_max_net_usage = 0;

    vector<string> tx_permission;

    const fc::string icp_logger_name("icp_relay");
     fc::logger icp_logger;
   std::string icp_peer_log_format;

   template<class enum_type, class=typename std::enable_if<std::is_enum<enum_type>::value>::type>
   inline enum_type& operator|=(enum_type& lhs, const enum_type& rhs)
   {
      using T = std::underlying_type_t <enum_type>;
      return lhs = static_cast<enum_type>(static_cast<T>(lhs) | static_cast<T>(rhs));
   }

    template<typename T>
   T icp_dejsonify(const string& s) {
      return fc::json::from_string(s).as<T>();
   }


    template<typename T>
    fc::variant call( const std::string& url,
                  const std::string& path,
                  const T& v ) {
   try {
      auto sp = std::make_unique<eosio::client::http::connection_param>(context, client::http::parse_url(url) + path, no_verify ? false : true, headers);
      return eosio::client::http::do_http_call(*sp, fc::variant(v), print_request, print_response );
    }
   catch(boost::system::system_error& e) {
      if(url == eosio::url)
         std::cerr << client::localize::localized("Failed to connect to nodeos at ${u}; is nodeos running?", ("u", url)) << std::endl;
      else if(url == eosio::wallet_url)
         std::cerr << client::localize::localized("Failed to connect to keosd at ${u}; is keosd running?", ("u", url)) << std::endl;
      throw client::http::connection_exception(fc::log_messages{FC_LOG_MESSAGE(error, e.what())});
        }
    }


    template<typename T>
    fc::variant call( const std::string& path,
                  const T& v ) { return call( url, path, fc::variant(v) ); }

    template<>
    fc::variant call( const std::string& url,
                  const std::string& path) { return call( url, path, fc::variant() ); }


   auto abi_serializer_resolver = [](const name& account) -> optional<abi_serializer> {
   static unordered_map<account_name, optional<abi_serializer> > abi_cache;
   auto it = abi_cache.find( account );
   if ( it == abi_cache.end() ) {
      eosio::chain_apis::read_only::get_abi_params abi_param;
      abi_param.account_name= account;
      eosio::chain_apis::read_only readonlydata = app().get_plugin<chain_plugin>().get_read_only_api();
      eosio::chain_apis::read_only::get_abi_results  abi_results=readonlydata.get_abi( abi_param );
      optional<abi_serializer> abis;
      if( abi_results.abi.valid() ) {
         abis.emplace( *abi_results.abi, abi_serializer_max_time );
      } else {
         std::cerr << "ABI for contract " << account.to_string() << " not found. Action data will be shown in hex only." << std::endl;
      }
      abi_cache.emplace( account, abis );

      return abis;
   }

   return it->second;
    };

    

   static appbase::abstract_plugin& _eoc_relay_plugin = app().register_plugin<eoc_relay_plugin>();
    
   
      eoc_relay_plugin::eoc_relay_plugin(){context = eosio::client::http::create_http_context();
      m_nblockno=1;
      m_nnodeid=1;}
      eoc_relay_plugin::~eoc_relay_plugin(){}

      void eoc_relay_plugin::set_program_options(options_description&, options_description& cfg) {
            cfg.add_options()
          ("icp-relay-endpoint", bpo::value<string>()->default_value("0.0.0.0:8899"), "The endpoint upon which to listen for incoming connections")
          ("icp-relay-address", bpo::value<string>()->default_value("localhost:8899"), "The localhost addr")
       ("icp-relay-threads", bpo::value<uint32_t>(), "The number of threads to use to process network messages")
       ("icp-relay-connect", bpo::value<vector<string>>()->composing(), "Remote endpoint of other node to connect to (may specify multiple times)")
       ("icp-relay-peer-chain-id", bpo::value<string>(), "The chain id of icp peer")
       ("icp-relay-peer-contract", bpo::value<string>()->default_value("cochainioicp"), "The peer icp contract account name")
       ("icp-relay-local-contract", bpo::value<string>()->default_value("cochainioicp"), "The local icp contract account name")
       ("icp-relay-signer", bpo::value<string>()->default_value("cochainrelay@active"), "The account and permission level to authorize icp transactions on local icp contract, as in 'account@permission'")
        ( "p2p-max-nodes-per-host", bpo::value<int>()->default_value(def_max_nodes_per_host), "Maximum number of client nodes from any single IP address")
        ( "agent-name", bpo::value<string>()->default_value("\"EOS Test Agent\""), "The name supplied to identify this node amongst the peers.")
        ( "icp-allowed-connection", bpo::value<vector<string>>()->multitoken()->default_value({"any"}, "any"), "Can be 'any' or 'producers' or 'specified' or 'none'. If 'specified', peer-key must be specified at least once. If only 'producers', peer-key is not required. 'producers' and 'specified' may be combined.")
        ( "peer-key", bpo::value<vector<string>>()->composing()->multitoken(), "Optional public key of peer allowed to connect.  May be used multiple times.")
        ( "peer-private-key", boost::program_options::value<vector<string>>()->composing()->multitoken(),
           "Tuple of [PublicKey, WIF private key] (may specify multiple times)")
         ( "max-clients", bpo::value<int>()->default_value(def_max_clients), "Maximum number of clients from which connections are accepted, use 0 for no limit")
         ( "connection-cleanup-period", bpo::value<int>()->default_value(def_conn_retry_wait), "number of seconds to wait before cleaning up dead connections")
         ( "max-cleanup-time-msec", bpo::value<int>()->default_value(10), "max connection cleanup time per cleanup call in millisec")
         ( "network-version-match", bpo::value<bool>()->default_value(false),
           "True to require exact match of peer network version.")
         ( "sync-fetch-span", bpo::value<uint32_t>()->default_value(def_sync_fetch_span), "number of blocks to retrieve in a chunk from any individual peer during synchronization")
         ( "max-implicit-request", bpo::value<uint32_t>()->default_value(def_max_just_send), "maximum sizes of transaction or block messages that are sent without first sending a notice")
         ( "use-socket-read-watermark", bpo::value<bool>()->default_value(false), "Enable expirimental socket read watermark optimization")
         ( "peer-log-format", bpo::value<string>()->default_value( "[\"${_name}\" ${_ip}:${_port}]" ),
           "The string used to format peers when logging messages about them.  Variables are escaped with ${<variable name>}.\n"
           "Available Variables:\n"
           "   _name  \tself-reported name\n\n"
           "   _id    \tself-reported ID (64 hex characters)\n\n"
           "   _sid   \tfirst 8 characters of _peer.id\n\n"
           "   _ip    \tremote IP address of peer\n\n"
           "   _port  \tremote port number of peer\n\n"
           "   _lip   \tlocal IP address connected to peer\n\n"
           "   _lport \tlocal port number connected to peer\n\n"),
        
        
        
        
        
        ("relaynodeid",bpo::value< int>()->default_value(0), "relay num begin to send");
          
      }

      vector<chain::permission_level> get_account_permissions(const vector<string> &permissions)
      {
          auto fixedPermissions = permissions | boost::adaptors::transformed([](const string &p) {
                                      vector<string> pieces;
                                      boost::algorithm::split(pieces, p, boost::algorithm::is_any_of("@"));
                                      if (pieces.size() == 1)
                                          pieces.push_back("active");
                                      return chain::permission_level{.actor = pieces[0], .permission = pieces[1]};
                                  });
          vector<chain::permission_level> accountPermissions;
          boost::range::copy(fixedPermissions, back_inserter(accountPermissions));
          return accountPermissions;
      }

     
     void eoc_relay_plugin::init_eoc_relay_plugin(const variables_map&options)
     {

         //peer_log_format = options.at( "peer-log-format" ).as<string>();

         relay_->network_version_match = options.at( "network-version-match" ).as<bool>();

       
         relay_->sync_master.reset( new icp::icp_sync_manager( options.at( "sync-fetch-span" ).as<uint32_t>()));
         relay_->connector_period = std::chrono::seconds( options.at( "connection-cleanup-period" ).as<int>());
         relay_->max_cleanup_time_ms = options.at("max-cleanup-time-msec").as<int>();
         relay_->txn_exp_period = def_txn_expire_wait;
         relay_->resp_expected_period = def_resp_expected_wait;
        // relay_->dispatcher->just_send_it_max = options.at( "max-implicit-request" ).as<uint32_t>();
         relay_->max_client_count = options.at( "max-clients" ).as<int>();
         relay_->max_nodes_per_host = options.at( "p2p-max-nodes-per-host" ).as<int>();
         relay_->num_clients = 0;
         relay_->started_sessions = 0;

         relay_->use_socket_read_watermark = options.at( "use-socket-read-watermark" ).as<bool>();

         relay_->resolver = std::make_shared<tcp::resolver>( std::ref( app().get_io_service()));
         
         if( options.count( "icp-relay-endpoint" )) {
            relay_->p2p_address = options.at( "icp-relay-endpoint" ).as<string>();
            auto host = relay_->p2p_address.substr( 0, relay_->p2p_address.find( ':' ));
            auto port = relay_->p2p_address.substr( host.size() + 1, relay_->p2p_address.size());
            idump((host)( port ));
            tcp::resolver::query query( tcp::v4(), host.c_str(), port.c_str());
            // Note: need to add support for IPv6 too?
            relay_->listen_endpoint = *relay_->resolver->resolve( query );
            relay_->acceptor.reset( new tcp::acceptor( app().get_io_service()));
            
         } else {
            if( relay_->listen_endpoint.address().to_v4() == address_v4::any()) {
               boost::system::error_code ec;
               auto host = host_name( ec );
               if( ec.value() != boost::system::errc::success ) {

                  FC_THROW_EXCEPTION( fc::invalid_arg_exception,
                                      "Unable to retrieve host_name. ${msg}", ("msg", ec.message()));

               }
               auto port = relay_->p2p_address.substr( relay_->p2p_address.find( ':' ), relay_->p2p_address.size());
               relay_->p2p_address = host + port;
            }
         }

         auto endpoint = options["icp-relay-endpoint"].as<string>();
                  relay_->endpoint_address_ = endpoint.substr(0, endpoint.find(':'));
                  relay_->endpoint_port_ = static_cast<uint16_t>(std::stoul(endpoint.substr(endpoint.find(':') + 1, endpoint.size())));
                  ilog("icp_relay_plugin listening on ${host}:${port}", ("host", relay_->endpoint_address_)("port", relay_->endpoint_port_));


     }

      void eoc_relay_plugin::plugin_initialize(const variables_map& options) {
          try
          {
              if (options.count("relaynodeid") > 0)
              {
                  // Handle the option
                  m_nnodeid = options["relaynodeid"].as<int>();     
              }
              else
              {
                  m_nnodeid = 0;
              }

              relay_ = std::make_shared<icp::relay>();
              relay_->chain_plug = app().find_plugin<chain_plugin>();
                  EOS_ASSERT(relay_->chain_plug, chain::missing_chain_plugin_exception, "");

                  if (options.count("icp-relay-connect"))
                  {
                      relay_->connect_to_peers_ = options.at("icp-relay-connect").as<vector<string>>();
                      relay_->supplied_peers= relay_->connect_to_peers_;
                  }

                  FC_ASSERT(options.count("icp-relay-peer-chain-id"), "option --icp-relay-peer-chain-id must be specified");
                  relay_->local_contract_ = account_name(options.at("icp-relay-local-contract").as<string>());
                  relay_->peer_contract_ = account_name(options.at("icp-relay-peer-contract").as<string>());
                  relay_->peer_chain_id_ = chain_id_type(options.at("icp-relay-peer-chain-id").as<string>());
                  relay_->signer_ = get_account_permissions(vector<string>{options.at("icp-relay-signer").as<string>()});
                   if( options.count( "agent-name" )) {
                     relay_->user_agent_name = options.at( "agent-name" ).as<string>();
                    }

                if( options.count( "peer-key" )) {
                    const std::vector<std::string> key_strings = options["peer-key"].as<std::vector<std::string>>();
                    for( const std::string& key_string : key_strings ) {
                        relay_->allowed_peers.push_back( icp_dejsonify<chain::public_key_type>( key_string ));
                    }
                }

                if( options.count( "peer-private-key" )) {
                    const std::vector<std::string> key_id_to_wif_pair_strings = options["peer-private-key"].as<std::vector<std::string>>();
                    for( const std::string& key_id_to_wif_pair_string : key_id_to_wif_pair_strings ) {
                    auto key_id_to_wif_pair = icp_dejsonify<std::pair<chain::public_key_type, std::string> >(
                     key_id_to_wif_pair_string );
                    relay_->private_keys[key_id_to_wif_pair.first] = fc::crypto::private_key( key_id_to_wif_pair.second );
                    }
                }

              relay_->chain_id = app().get_plugin<chain_plugin>().get_chain_id();
              fc::rand_pseudo_bytes( relay_->node_id.data(), relay_->node_id.data_size());
              ilog( "my node_id is ${id}", ("id", relay_->node_id));

             if( options.count( "icp-allowed-connection" )) {
            const std::vector<std::string> allowed_remotes = options["allowed-connection"].as<std::vector<std::string>>();
            for( const std::string& allowed_remote : allowed_remotes ) {
               if( allowed_remote == "any" )
                  relay_->allowed_connections |= icp::relay::Any;
               else if( allowed_remote == "producers" )
                  relay_->allowed_connections |= icp::relay::Producers;
               else if( allowed_remote == "specified" )
                  relay_->allowed_connections |= icp::relay::Specified;
               else if( allowed_remote == "none" )
                  relay_->allowed_connections = icp::relay::None;
            }
         }


                init_eoc_relay_plugin(options);
        }
           
            FC_LOG_AND_RETHROW()
      }

     
      void eoc_relay_plugin::plugin_startup() {
            ilog("starting eoc_relay_plugin");
            //sendstartaction();
             auto& chain = app().get_plugin<chain_plugin>().chain();
            FC_ASSERT(chain.get_read_mode() != chain::db_read_mode::IRREVERSIBLE, "icp is not compatible with \"irreversible\" read_mode");

            relay_->start();
            chain::controller&cc = relay_->chain_plug->chain();
            {
                cc.applied_transaction.connect( boost::bind(&icp::relay::on_applied_transaction, relay_.get(), _1));
                cc.accepted_block_with_action_digests.connect( boost::bind(&icp::relay::on_accepted_block, relay_.get(), _1) );
                cc.irreversible_block.connect(boost::bind(&icp::relay::on_irreversible_block, relay_.get(), _1) );
            }

             relay_->start_monitors();

      for( auto seed_node : relay_->connect_to_peers_ ) {
         connect( seed_node );
      }

      if(fc::get_logger_map().find(icp_logger_name) != fc::get_logger_map().end())
         icp_logger = fc::get_logger_map()[icp_logger_name];


   // Make the magic happen
      }

     icp::read_only eoc_relay_plugin::get_read_only_api()
     {
        return relay_->get_read_only_api();
     }
   icp::read_write eoc_relay_plugin::get_read_write_api()
   {
       return relay_->get_read_write_api();
   }

      void eoc_relay_plugin::plugin_shutdown() {
        // OK, that's enough magic
        try {
         ilog( "shutdown.." );
         relay_->done = true;
         if( relay_->acceptor ) {
            ilog( "close acceptor" );
            relay_->acceptor->close();

            ilog( "close ${s} connections",( "s",relay_->connections.size()) );
            auto cons = relay_->connections;
            for( auto con : cons ) {
               relay_->close( con);
            }

            relay_->acceptor.reset(nullptr);
         }
         ilog( "exit shutdown" );
        }
      FC_CAPTURE_AND_RETHROW()
      }

     string  eoc_relay_plugin::connect( const string& host )
     {
         if( relay_->find_connection( host ) )
         return "already connected";

      icp::icp_connection_ptr c = std::make_shared<icp::icp_connection>(host);
      //fc_dlog(logger,"adding new connection to the list");
      relay_->connections.insert( c );
      //fc_dlog(logger,"calling active connector");
      relay_->connect( c );
      return "added connection";
     }

      uint32_t eoc_relay_plugin::get_num_send()
   {
       return m_nblockno;
   }

   void eoc_relay_plugin::analy_dif_chainmsg(std::vector<chain::signed_block_ptr> & vecBlocks)
   {
      // ilog("begin analyDifChainMsg-------------------------");
       controller& cc = appbase::app().find_plugin<chain_plugin>()->chain();
       uint32_t lastnum = cc.last_irreversible_block_num();
       uint32_t headnum = m_nblockno;
       //ilog("begin head num is ${headnum}",("headnum", m_nNumSend));
       if(headnum > lastnum)
            return;
       if(lastnum > headnum+MAXSENDNUM)
            lastnum = headnum+MAXSENDNUM;
       for(uint32_t i = headnum; i <= lastnum; i++)
       {
           signed_block_ptr blockptr = cc.fetch_block_by_number(i);
           vecBlocks.push_back(blockptr);
           //ilog("block num is : ${blocknum}",("blocknum", blockptr->block_num()));
       }    
       m_nblockno = lastnum+1;
       //ilog("end head num is ${headnum}",("headnum", m_nNumSend));
   }

   void eoc_relay_plugin::recv_dif_chainmsg(const difchain_message &msg)
   {
       // ilog("begin recvDifChainMsg-------------------------");
         ilog( "message network_version is ${network_version}", ("network_version", msg.network_version));
         ilog( "message chainid is ${chainid}",("chainid",msg.chain_id));
         ilog( "message blocknum is ${blocknum}",("blocknum",msg.signedblock.block_num()));
        // ilog("message blockid is ${blockid}",("blockid",msg.signedblock.id()));
        // ilog("message blockadder is ${blockadder}",("blockadder",msg.blockadder));
        uint32_t blocknum = msg.signedblock.block_num();
        blockcontainer::index<indexblocknum>::type& indexOfBlock = m_container.get<indexblocknum>();
        auto iterfind = indexOfBlock.find(blocknum);
        //find block num
        if(iterfind != indexOfBlock.end())
        {
            ilog( "blocknum  ${blocknum} has exist",("blocknum",blocknum));
            return;
        }
        //indexofblock empty
        if(indexOfBlock.empty())
        {
            signed_blockplus blockplus;
            blockplus.m_nNum = blocknum;
            blockplus.m_block = msg.signedblock;
            m_container.insert(blockplus);
            ilog( "insert blocknum ${blocknum} success ",("blocknum",blocknum));
            return;
        }
        auto iterMax = indexOfBlock.end();
        iterMax--;
        //current block is smaller than the max block in multi_index container
        if(iterMax->m_nNum > blocknum)
        {
            ilog( "blocknum  ${blocknum} smaller than latest",("blocknum",blocknum));
            return;
        }  
        signed_blockplus blockplus;
        blockplus.m_nNum = blocknum;
        blockplus.m_block = msg.signedblock;
        m_container.insert(blockplus);
        ilog( "insert blocknum ${blocknum} success ",("blocknum",blocknum));
       // ilog("recvDifChainMsg success-------------------------");
   }

    
    fc::variant eoc_relay_plugin::json_from_file_or_string(const string& file_or_str, fc::json::parse_type ptype )
    {
        std::regex r("^[ \t]*[\{\[]");
        if ( !std::regex_search(file_or_str, r) && fc::is_regular_file(file_or_str) ) {
            return fc::json::from_file(file_or_str, ptype);
            } else {
      return fc::json::from_string(file_or_str, ptype);
        }
    }

    void eoc_relay_plugin::transaction_next(const fc::static_variant<fc::exception_ptr, eosio::chain::transaction_trace_ptr>& param)
    {
        ilog("transaction_next call success !!!!");
    }

    icp::relay* eoc_relay_plugin::get_relay_pointer()
     {
         return relay_.get();
     }

    void eoc_relay_plugin::get_relay_info()
    {
        eosio::chain_apis::read_only readonlydata = app().get_plugin<chain_plugin>().get_read_only_api();
     eosio::chain_apis::read_only::get_table_rows_params tableparam;
     //curl  http://127.0.0.1:8888/v1/chain/get_table_rows -X POST -d '{"scope":"inita", "code":"currency", "table":"account", "json": true}'
     try{
        tableparam.code="twinklerelay";
         tableparam.table="blocksends";
        tableparam.scope="twinklerelay";
        tableparam.json = true;
        read_only::get_table_rows_result  tableres= readonlydata.get_table_rows(tableparam);
        //ilog( "insert tableresrow size  ${tablerowsize} success ",("tablerowsize",tableres.rows.size() ) );
        for(unsigned int i =0; i < tableres.rows.size(); i++)
        {
            ilog("row index is ${index}",("index",i));
            ilog( "rows data is ${rowsdata} ",("rowsdata",tableres.rows[i]) );
        }

     }
     catch(const fc::exception& e)
     {
         edump((e.to_detail_string() ));
     }
     
    }

   void eoc_relay_plugin::get_info()
   { 
     eosio::chain_apis::read_only readonlydata = app().get_plugin<chain_plugin>().get_read_only_api();
     eosio::chain_apis::read_only::get_table_rows_params tableparam;
     //curl  http://127.0.0.1:8888/v1/chain/get_table_rows -X POST -d '{"scope":"inita", "code":"currency", "table":"account", "json": true}'
     try{
        tableparam.code="eosio.token";
         tableparam.table="accounts";
        tableparam.scope="twinkle11111";
        tableparam.json = true;
        read_only::get_table_rows_result  tableres= readonlydata.get_table_rows(tableparam);
        //ilog( "insert tableresrow size  ${tablerowsize} success ",("tablerowsize",tableres.rows.size() ) );
        for(unsigned int i =0; i < tableres.rows.size(); i++)
        {
            ilog("row index is ${index}",("index",i));
            ilog( "rows data is ${rowsdata} ",("rowsdata",tableres.rows[0]) );
        }

     }
     catch(const fc::exception& e)
     {
         edump((e.to_detail_string() ));
     }
     
     //1 
    //  eosio::chain_apis::read_write readwritedata = app().get_plugin<chain_plugin>().get_read_write_api();
    //  read_write::push_transaction_params params;
    //  eosio::chain::plugin_interface::next_function<read_write::push_transaction_results> next;
    //  readwritedata.push_transaction(params, next);
   
   //sendrelayaction();
   //sendstartaction();
    
   }

    void eosio::eoc_relay_plugin::sendrelayaction()
    {
        try{
            const std::string fromid = "\"eosio\"";
            const std::string toid = "\"twinkle11111\"";
            const std::string quantity = "\"1.0000 SYS\"";
            const std::string data = "{\"from\":" + fromid + "," + "\"to\":" + toid + "," +
                                     "\"quantity\":" + quantity + "," + "\"memo\":test}";

            ilog("action data is ${actiondata}", ("actiondata", data));
            fc::variant action_args_var;
            action_args_var = json_from_file_or_string(data, fc::json::relaxed_parser);
            std::vector<eosio::chain::permission_level> auth;
            eosio::chain::permission_level plv;
            plv.permission = "active";
            plv.actor = "eosio";
            auth.push_back(plv);
            eosio::chain::account_name contract_account("eosio.token");
            eosio::chain::action_name action("transfer");
            eosio::chain::bytes databytes = variant_to_bin(contract_account, action, action_args_var);
            if (databytes.empty())
            {
                ilog("databytes is empty???????????????????????");
                return;
            }
            send_actions({chain::action{auth, contract_account, action, databytes}});
        }
        catch(const fc::exception& e)
        {
             edump((e.to_detail_string() ));
        }
    }

    void eosio::eoc_relay_plugin::sendstartaction()
    {
         try{
            int64_t blocknum = 1;
            std::stringstream mystream;
            mystream << m_nnodeid;
            std::string nodeidstr;
            mystream >> nodeidstr;
            mystream.clear();
            mystream.str();
            mystream << blocknum;
            std::string blocknumstr;
            mystream >> blocknumstr;
            const std::string data = "{\"nodeid\":" + nodeidstr + "," + "\"blockno\":" + blocknumstr+"}";

            ilog("action data is ${actiondata}", ("actiondata", data));
            fc::variant action_args_var;
            action_args_var = json_from_file_or_string(data, fc::json::relaxed_parser);
            std::vector<eosio::chain::permission_level> auth;
            eosio::chain::permission_level plv;
            plv.permission = "active";
            plv.actor = "twinklerelay";
            auth.push_back(plv);
            eosio::chain::account_name contract_account("twinklerelay");
            eosio::chain::action_name action("addnode");
            eosio::chain::bytes databytes = variant_to_bin(contract_account, action, action_args_var);
            if (databytes.empty())
            {
                ilog("databytes is empty???????????????????????");
                return;
            }
            send_actions({chain::action{auth, contract_account, action, databytes}});
        }
        catch(const fc::exception& e)
        {
             edump((e.to_detail_string() ));
        }
    }

     eosio::chain::bytes eosio::eoc_relay_plugin::variant_to_bin( const account_name& account, const action_name& action, const fc::variant& action_args_var ) {
        eosio::chain::bytes databytes;
        try{  
            auto abis = abi_serializer_resolver( account );
            if(abis.valid() == false )
                return databytes; 
            auto action_type = abis->get_action_type( action );
            if(action_type.empty() )
                return databytes;
            return abis->variant_to_binary( action_type, action_args_var, abi_serializer_max_time );

        }
        catch(const fc::exception& e)
        {
            edump((e.to_detail_string() ));
            return databytes;
        }
        catch(const boost::exception& e)
        {
            ilog("boost excpetion is ${exception}",("exception",boost::diagnostic_information(e).c_str()));
            return databytes;
        }
        
     }

    fc::variant eoc_relay_plugin::determine_required_keys(const signed_transaction& trx)
    {
         const auto& public_keys = call(wallet_url, eosio::client::http::wallet_public_keys);
            auto get_arg = fc::mutable_variant_object
           ("transaction", (transaction)trx)
           ("available_keys", public_keys);
            const auto& required_keys = call(eosio::client::http::get_required_keys, get_arg);
            return required_keys["required_keys"];
    }

    void eoc_relay_plugin::sign_transaction(signed_transaction& trx, fc::variant& required_keys, const chain_id_type& chain_id) {
            fc::variants sign_args = {fc::variant(trx), required_keys, fc::variant(chain_id)};
            const auto& signed_trx = call(wallet_url, eosio::client::http::wallet_sign_trx, sign_args);
            trx = signed_trx.as<signed_transaction>();
    }


   void eoc_relay_plugin::send_actions(std::vector<chain::action>&& actions, int32_t extra_kcpu, packed_transaction::compression_type compression  )
    {
        try{
            eosio::chain::packed_transaction_ptr packed_trx=std::make_shared<eosio::chain::packed_transaction>();
        eosio::chain::plugin_interface::next_function<eosio::chain::transaction_trace_ptr> next = std::bind(&eoc_relay_plugin::transaction_next,
         this,std::placeholders::_1);

        signed_transaction trx;
        trx.actions = std::forward<decltype(actions)>(actions);
        read_only::get_info_params infoparams;
        eosio::chain_apis::read_only readonlydata = app().get_plugin<chain_plugin>().get_read_only_api();
        read_only::get_info_results infores = readonlydata.get_info(infoparams);
        auto   tx_expiration = fc::seconds(30);
        trx.expiration = infores.head_block_time + tx_expiration;
        block_id_type ref_block_id = infores.last_irreversible_block_id;
        trx.set_reference_block(ref_block_id);
        uint8_t  tx_max_cpu_usage = 0;
        uint32_t tx_max_net_usage = 0;
        trx.max_cpu_usage_ms = tx_max_cpu_usage;
        trx.max_net_usage_words = (tx_max_net_usage + 7)/8;
        if (!tx_skip_sign) {
                 auto required_keys = determine_required_keys(trx);
                 sign_transaction(trx, required_keys, infores.chain_id);
             }
        ilog("begin packed  -----------------------");
        *packed_trx= packed_transaction(trx, compression);
         ilog("packed success !!!!!!!!!!!!!!!!!!!");
        app().get_plugin<producer_plugin>().process_transaction(packed_trx, true, next);

        }catch(const fc::exception& e)
        {
            edump((e.to_detail_string() ));
        }
        
    }


}
