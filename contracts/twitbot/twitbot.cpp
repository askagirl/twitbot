#include <eosiolib/eosio.hpp>
#include <eosiolib/currency.hpp>

using namespace eosio;
using namespace std;

class twitbot : public eosio::contract {
public:
    twitbot(action_name self) : contract(self) {}

    void tip(string from_twitter, string to_twitter, uint64_t quantity) {
        require_auth(_self);
        print("Tip");
    }

    void withdraw(string from_twitter, account_name to_eos) {
        require_auth(_self);

        // TODO: check amount

        action(permission_level{_self, N(active)}, N(eosio.token), N(transfer),
               make_tuple(_self, to_eos, 0, string(""))).send();

        // TODO: remove from table amount

        print("withdraw");
    }

    void claim(string from_twitter, account_name to_eos) {
        require_auth(_self);
        print("claim");
    }

    void apply(account_name contract, account_name act) {

        if( act == N(transfer) ) {
            _transfer(unpack_action_data<currency::transfer>(), contract);
            return;
        }

        auto &thiscontract = *this;
        switch (act) {
            EOSIO_API(twitbot, (tip)(withdraw)(claim))
        };
    }

    void _transfer(const currency::transfer& trs, account_name code) {
        print("transfer received");
    }

private:

    //@abi table account i64
    struct account {
        account_name name;
        string twitter;
        uint64_t balance = 0;

        uint64_t primary_key() const { return name; }

        static key256 key(string twitter) {
            return key256::make_from_word_sequence<uint64_t>(string_to_name(twitter.c_str()));
        }

        key256 get_key() const { return key(twitter); }

        EOSLIB_SERIALIZE(account, (name)(twitter)(balance))
    };

    typedef eosio::multi_index<N(account), account,
            eosio::indexed_by<N(bytwitter), eosio::const_mem_fun<account, key256, &account::get_key> >
    > account_index_type;
};

extern "C" {
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        auto self = receiver;
        twitbot twit(self);
        twit.apply(code, action);
        eosio_exit(0);
    }
}