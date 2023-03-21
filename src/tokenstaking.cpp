#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/action.hpp>
#include <eosio/transaction.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/time.hpp>


using namespace eosio;
using std::string;
using std::vector;

name tokencontract = name("eosio.token");


CONTRACT tokenstaking : public eosio::contract {
 public:
  tokenstaking( name receiver, name code, datastream<const char*> ds ):
    contract(receiver, code, ds),
    _users(receiver, receiver.value),
    _config(receiver, receiver.value)
    {}


    [[eosio::on_notify("eosio.token::transfer")]]
    void transfertoken(name from,name to, asset quantity, string memo){
        auto itr_users = _users.find(from.value);

        uint64_t time = stoi(memo);
        uint64_t current_time = current_time_point().sec_since_epoch();
        auto itr_apy = _config.find(get_self().value);
        double apy = 0;

        for ( int i = 0; i < itr_apy->APY.size(); i++ ) {
            if ( itr_apy->APY[i].locktime == time ) {
                apy = itr_apy->APY[i].apy;
            }
        }
        check ( apy != 0,"Locktime error");
        if ( itr_users == _users.end() ) {

            vector<stake_tokens> stake_tokens;
            stake_tokens.push_back({
                .locktime = current_time + time,
                .starttime = current_time,
                .token = quantity,
                .apy = apy
            });

            _users.emplace(get_self(), [&](auto& row) {
                row.username = from;
                row.stake = stake_tokens;
            });
        } else {
            _users.modify(itr_users, get_self(), [&](auto& row) {
                row.stake.push_back({
                    .locktime = current_time + time,
                    .starttime = current_time,
                    .token = quantity,
                    .apy = apy
                });
            });
        }
    }

    ACTION claim ( name username, uint64_t id ) {
        require_auth(username);

        auto itr_users = _users.require_find(username.value,"Error with table find");
        uint64_t current_time = current_time_point().sec_since_epoch();
        check ( current_time < itr_users->stake[id].locktime,"Timelock error");

        
        action(
          permission_level{name(get_self()), name("active")},
          tokencontract,
          "transfer"_n,
          make_tuple(get_self(), username,itr_users->stake[id].token , string("Claim tokens")))
        .send();
    }

    ACTION addapy( uint64_t locktime, double apy ) {
        require_auth(get_self());

        auto itr_config = _config.find(get_self().value);

        if ( itr_config == _config.end() ) {
            _config.emplace(get_self(), [&](auto& row) {
                row.username = get_self();
                row.APY.push_back({
                    .locktime = locktime,
                    .apy = apy
                });
            });
        } else {
            _config.modify(itr_config, get_self(), [&](auto& row) {
                row.APY.push_back({
                    .locktime = locktime,
                    .apy = apy
                });
            });
        }
    }

 private:

    struct stake_tokens
    {   
        uint64_t locktime;
        uint64_t starttime;
        asset token;
        double apy;
    };

    TABLE user {
        name username;
        vector<stake_tokens> stake;

        auto primary_key() const { return username.value; }
      };

    typedef multi_index<name("users"), user> users;
    users _users;

    struct token_apy
    {   
        uint64_t locktime;
        double apy;
    };


    TABLE config { 
        name username;
        vector<token_apy> APY;
        
        auto primary_key() const { return username.value; }
    };

    typedef multi_index<name("config"), config> configs;
    configs _config;

};