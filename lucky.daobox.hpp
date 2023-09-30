#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>

#include "src/structs.hpp"

using namespace eosio;
using namespace std;
using std::vector;

class [[eosio::contract("lucky.daobox")]] lucky : public contract {
   public:
    using contract::contract;

    // CONSTANTS
    const uint16_t RATE_PRECISION = 10000;
    const uint16_t MAX_FEE_RATE = 3000;  // maximum rate of 30%
    const uint8_t MAX_LITTLE_WINNERS = 10;

    /**
   *
   * ## TABLE `config`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` -  last pool id
   * - `{uint64_t} fee_rate` -  percentage of fees_account transferred(pips
   * 100/10000 of 1%)
   * - `{name} admin_account` - admin account
   * - `{name} fees_account` - receiving service charge account
   *
   * ### example
   *
   * ```json
   * {
   *     "pool_id": "2",
   *     "fee_rate": "10",
   *     "admin_account": "admin.daobox",
   *     "fees_account": "fees.daobox"
   *  }
   * ```
   * */
    struct [[eosio::table("config")]] config_row {
        uint64_t pool_id;
        uint16_t fee_rate = 10;
        name admin_account = "admin.daobox"_n;
        name fees_account = "fees.daobox"_n;
    };
    typedef eosio::singleton<"config"_n, config_row> config_table;

    /**
   *  pools
   * ## TABLE `pools`
   *
   * ### params
   *
   * - `{uint64_t} id` - (primary key) reward pool id
   * - `{uint64_t} win_num` - win number
   * - `{uint64_t} winner` - prize-winner
   * - `{asset} rewards_quantity` - the rewards quantity
   * - `{uint64_t} vote_start` - voting start time
   * - `{uint64_t} vote_end` - voting end time
   * - `{uint64_t} take_end` - number pickup end time
   * - `{uint64_t} tickets` - total tickets
   * - `{extended_symbol} rewards` - rewards token
   * - `{uint64_t} last_take_num` - ratio of exchange
   *
   * ### example
   *
   * ```json
   * {
   *   "id": 2,
   *   "win_num": 0,
   *   "winner": "holder1",
   *   "rewards_quantity": "100.0000 EOS",
   *   "vote_start": "2022-12-30T02:30:00",
   *   "vote_end": "2022-12-30T02:33:00",
   *   "take_end": "2022-12-30T02:45:00",
   *   "tickets": 12,
   *   "rewards": {
   *     "sym": "4,EOS",
   *     "contract": "eosio.token"
   *   },
   *   "last_take_num": 9
   * }
   * ```
   * */
    struct [[eosio::table("pools")]] pool_row {
        uint64_t id;
        uint64_t win_num;
        name winner;
        asset rewards_quantity;
        time_point_sec vote_start;
        time_point_sec vote_end;
        time_point_sec take_end;
        uint64_t tickets;
        extended_symbol rewards;
        uint64_t last_take_num;
        uint64_t primary_key() const { return id; }
    };
    typedef eosio::multi_index<"pools"_n, pool_row> pool_table;

    /**
   *
   * ## TABLE `little.award`
   *
   * ### params
   *
   * - `{name} holder` - ticket holder
   * - `{asset} rewards_quantity` - the rewards quantity
   *
   * ### example
   *
   * ```json
   * {
   *   "holder": "holder1",
        "rewards_quantity": "10.0000 EOS"
   * }
   * ```
   * */
    struct [[eosio::table("little.award")]] little_award_row {
        name holder;
        asset rewards_quantity;
        uint64_t primary_key() const { return holder.value; }
    };
    typedef eosio::multi_index<"little.award"_n, little_award_row> little_award_table;

    /**
   *
   * ## TABLE `holders1`
   *
   * ### params
   *
   * - `{name} holder` - ticket holder
   * - `{uint64_t} sort` - sort number
   * - `{uint64_t} tickets` - the number of tickets that can be held
   *
   * ### example
   *
   * ```json
   * {
   *   "holder": "holder1",
   *   "sort": 2,
   *   "tickets": 9,
   * }
   * ```
   * */
    struct [[eosio::table("holders1")]] holder1_row {
        name holder;
        uint64_t sort;
        uint64_t tickets;
        uint64_t primary_key() const { return holder.value; }
        uint64_t bysort() const { return sort; }
    };
    typedef eosio::multi_index<"holders1"_n, holder1_row, eosio::indexed_by<"bysort"_n, eosio::const_mem_fun<holder1_row, uint64_t, &holder1_row::bysort>>> holder1_table;

    /**
   *
   * ## TABLE `holders2`
   *
   * ### params
   *
   * - `{name} holder` - ticket holder
   * - `{uint64_t} sort` - sort number
   * - `{uint64_t} tickets` - the number of tickets that can be held
   * - `{uint64_t} from` - starting ticket number
   * - `{uint64_t} to` - end ticket number
   *
   * ### example
   *
   * ```json
   * {
   *   "holder": "holder1",
   *   "sort": 2,
   *   "tickets": 9,
   *   "from": 1,
   *   "to": 9
   * }
   * ```
   * */
    struct [[eosio::table("holders2")]] holder2_row {
        name holder;
        uint64_t sort;
        uint64_t tickets;
        uint64_t from;
        uint64_t to;
        uint64_t primary_key() const { return holder.value; }
        uint64_t byto() const { return to; }
        uint64_t bysort() const { return sort; }
    };
    typedef eosio::multi_index<"holders2"_n,
                               holder2_row,
                               eosio::indexed_by<"byto"_n, eosio::const_mem_fun<holder2_row, uint64_t, &holder2_row::byto>>,
                               eosio::indexed_by<"bysort"_n, eosio::const_mem_fun<holder2_row, uint64_t, &holder2_row::bysort>>>
        holder2_table;

    /**
   * ## ACTION `init`
   *
   * > Initialize the contract
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{uint16_t} fee_rate` - ratio of service charge
   * - `{name} admin_account` - lucky.daobox admin contract
   * - `{name} fees_account` - lucky.daobox fees contract
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action lucky.daobox init '[10, "admin.daobox", "fees.daobox"]'
   * -p admin.daobox
   * ```
   */
    [[eosio::action]]
    void init(const uint16_t fee_rate, const name admin_account, const name fees_account);

    /**
   * ## ACTION `createpool`
   *
   * > Create pool
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{time_point_sec} vote_start` - voting start time
   * - `{time_point_sec} vote_end` - voting end time
   * - `{time_point_sec} take_end` - number pickup end time
   * - `{extended_symbol} rewards` - rewards token
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action lucky.daobox createpool '[1672281118, 1672885918,
   * 1672972318, { sym: '4,EOS', contract: 'eosio.token' }]' -p admin.daobox
   * ```
   */
    [[eosio::action]]
    void createpool(const time_point_sec vote_start, const time_point_sec vote_end, const time_point_sec take_end, extended_symbol rewards);

    /**
   * ## ACTION `modifypool`
   *
   * > Modify pool
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{uint16_t} pool_id` - reward pool id
   * - `{time_point_sec} vote_start` - voting start time
   * - `{time_point_sec} vote_end` - voting end time
   * - `{time_point_sec} take_end` - number pickup end time
   * - `{extended_symbol} rewards` - rewards token
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action lucky.daobox modifypool '[1, 1672281118, 1672885918,
   * 1672972318, { sym: '4,EOS', contract: 'eosio.token' }]' -p admin.daobox
   * ```
   */
    [[eosio::action]]
    void modifypool(const uint64_t pool_id, const time_point_sec vote_start, const time_point_sec vote_end, const time_point_sec take_end, extended_symbol rewards);

    /**
   * ## ACTION `delpool`
   *
   * > Delete pool
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action lucky.daobox delpool '[1]' -p lucky.daobox
   * ```
   */
    [[eosio::action]]
    void delpool(const uint64_t pool_id);

    /**
   * ## ACTION `upload`
   *
   * > Initialize the rewards contract
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{vector<upload_item>} holder_tickets` - number of votes held by users
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action lucky.daobox upload '[1, [ { holder: "account1",
   * tickets: 100, sort: 1 }]]' -p admin.daobox
   * ```
   */
    [[eosio::action]]
    void upload(const uint64_t pool_id, const vector<upload_item> holder_tickets);
    /**
   * ## ACTION `take`
   *
   * > Take number
   *
   * - **authority**: `holder`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{name} holder` - ticket holder
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action daobox take '[1, "holder1"]' -p holder1
   * ```
   */
    [[eosio::action]]
    void take(const uint64_t pool_id, const name holder);

    /**
   * ## ACTION `take2`
   *
   * > Take number
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{uint64_t} max_rows` - erase max row
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action daobox take2 '[1, 50]' -p
   * admin.daobox
   * ```
   */
    [[eosio::action]]
    void take2(const uint64_t pool_id, const optional<uint64_t> max_rows);

    /**
   * ## ACTION `draw`
   *
   * > Draw the winning numbers of a lottery
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action daobox draw '[1]' -p admin.daobox
   * ```
   */
    [[eosio::action]]
    void draw(const uint64_t pool_id);

    /**
   * ## ACTION `draw2`
   *
   * > Draw the winning numbers of a lottery
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action daobox draw2 '[1]' -p admin.daobox
   * ```
   */
    [[eosio::action]]
    void draw2(const uint64_t pool_id);
    /**
   * ## ACTION `recycle`
   *
   * > Erase holder table data
   *
   * - **authority**: `admin.daobox`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{uint64_t} max_rows` - erase max row
   *
   * ### Example
   *
   * ```bash
   * $ cleos push action daobox recycle '[1, 50]' -p admin.daobox
   * ```
   */
    [[eosio::action]]
    void recycle(const uint64_t pool_id, const optional<uint64_t> max_rows);

    /**
   * ## ACTION `createpoollog`
   *
   * > Generates a log when a pool is created.
   *
   * - **authority**: `_self`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{time_point_sec} vote_start` - voting start time
   * - `{time_point_sec} vote_end` - voting end time
   * - `{time_point_sec} take_end` - number pickup end time
   * - `{extended_symbol} rewards` - rewards token
   *
   * ### example
   *
   * ```json
   * {
   *   "pool_id": 1,
   *   "vote_start": "2022-12-30T02:30:00",
   *   "vote_end": "2022-12-30T02:33:00",
   *   "take_end": "2022-12-30T02:45:00",
   *   "rewards": {
   *     "sym": "4,EOS",
   *     "contract": "eosio.token"
   *   }
   * }
   * ```
   */
    [[eosio::action]]
    void createpoollog(const uint64_t pool_id, const time_point_sec vote_start, const time_point_sec vote_end, const time_point_sec take_end, extended_symbol rewards)
    {
        require_auth(_self);
    };

    /**
   * ## ACTION `modifypoollog`
   *
   * > Generates a log when a pool is modify.
   *
   * - **authority**: `_self`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{time_point_sec} vote_start` - voting start time
   * - `{time_point_sec} vote_end` - voting end time
   * - `{time_point_sec} take_end` - number pickup end time
   * - `{extended_symbol} rewards` - rewards token
   *
   * ### example
   *
   * ```json
   * {
   *   "pool_id": 1,
   *   "vote_start": "2022-12-30T02:30:00",
   *   "vote_end": "2022-12-30T02:33:00",
   *   "take_end": "2022-12-30T02:45:00",
   *   "rewards": {
   *     "sym": "4,EOS",
   *     "contract": "eosio.token"
   *   }
   * }
   * ```
   */
    [[eosio::action]]
    void modifypoollog(const uint64_t pool_id, const time_point_sec vote_start, const time_point_sec vote_end, const time_point_sec take_end, extended_symbol rewards)
    {
        require_auth(_self);
    };

    /**
   * ## ACTION `delpoollog`
   *
   * > Generates a log when a pool is delete.
   *
   * - **authority**: `_self`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   *
   * ### example
   *
   * ```json
   * {
   *   "pool_id": 1,
   * }
   * ```
   */
    [[eosio::action]]
    void delpoollog(const uint64_t pool_id)
    {
        require_auth(_self);
    };

    /**
   * ## ACTION `uploadlog`
   *
   * > Generates a log when upload ticket.
   *
   * - **authority**: `_self`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{vector<upload_item>} holder_tickets` -
   *
   * ### example
   *
   * ```json
   * {
   *     "pool_id": "myprotocol",
   *     "holder_tickets": [{
   *        "holder": "holder1",
   *        "sort": 1,
   *        "tickets": 8,
   *      },
   *      {
   *        "holder": "holder2",
   *        "sort": 2,
   *        "tickets": 9,
   *      },
   *    ]
   * }
   * ```
   */
    [[eosio::action]]
    void uploadlog(const uint64_t pool_id, const vector<upload_item> holder_tickets)
    {
        require_auth(_self);
    };

    /**
   * ## ACTION `takelog`
   *
   * > Generates a log when take a number.
   *
   * - **authority**: `_self`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{name} holder` - ticket holder
   * - `{uint64_t} from` - starting ticket number
   * - `{uint64_t} to` - end ticket number
   *
   * ### example
   *
   * ```json
   * {
   *     "pool_id": 1,
   *     "holder": "holder1",
   *     "from": 10,
   *     "to": 100
   * }
   * ```
   */
    [[eosio::action]]
    void takelog(const uint64_t pool_id, const name holder, const uint64_t from, const uint64_t to)
    {
        require_auth(_self);
    };
    /**
   * ## ACTION `drawlog`
   *
   * > Generates a log when draw the winning numbers.
   *
   * - **authority**: `_self`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{uint64_t} win_num` - winning number
   * - `{uint64_t} winner` - prize-winner
   * - `{asset} fees_quantity` - the fees quantity
   * - `{asset} rewards_quantity` - the rewards quantity
   * ### example
   *
   * ```json
   * {
   *     "pool_id": 1,
   *     "win_num": 99,
   *     "winner": "holder1",
   *     "fees_quantity": "100.0000 EOS",
   *     "rewards_quantity": "100.0000 EOS"
   * }
   * ```
   */
    [[eosio::action]]
    void drawlog(const uint64_t pool_id, const uint64_t win_num, const name winner, const asset fees_quantity, const asset rewards_quantity)
    {
        require_auth(_self);
    };

    /**
   * ## ACTION `draw2log`
   *
   * > Generates a log when draw the winning numbers.
   *
   * - **authority**: `_self`
   *
   * ### params
   *
   * - `{uint64_t} pool_id` - reward pool id
   * - `{vector<name>} winners` - prize-winners
   * - `{asset} feess_quantity` - the fees quantity
   * - `{asset} rewards_quantity` - the rewards quantity
   * ### example
   *
   * ```json
   * {
   *     "pool_id": 1,
   *     "winners": ["holder1"],
   *     "fees_quantity": "100.0000 EOS",
   *     "rewards_quantity": "100.0000 EOS"
   * }
   * ```
   */
    [[eosio::action]]
    void draw2log(const uint64_t pool_id, const vector<name> winners, const asset fees_quantity, const asset rewards_quantity)
    {
        require_auth(_self);
    };

#ifdef DEBUG
    [[eosio::action]]
    void updatepool(const uint64_t pool_id, const time_point_sec vote_start, const time_point_sec vote_end, const time_point_sec take_end, extended_symbol rewards);

    // @debug
    [[eosio::action]]
    void cleartable(const name table_name, const optional<name> scope, const optional<uint64_t> scope_num, const optional<uint64_t> max_rows);
#endif

    // action wrappers
    using init_action = eosio::action_wrapper<"init"_n, &lucky::init>;
    using createpool_action = eosio::action_wrapper<"createpool"_n, &lucky::createpool>;
    using modifypool_action = eosio::action_wrapper<"modifypool"_n, &lucky::modifypool>;
    using delpool_action = eosio::action_wrapper<"delpool"_n, &lucky::delpool>;
    using upload_action = eosio::action_wrapper<"upload"_n, &lucky::upload>;
    using take_action = eosio::action_wrapper<"take"_n, &lucky::take>;
    using take2_action = eosio::action_wrapper<"take2"_n, &lucky::take2>;
    using draw_action = eosio::action_wrapper<"draw"_n, &lucky::draw>;
    using draw2_action = eosio::action_wrapper<"draw2"_n, &lucky::draw2>;

    using createpoollog_action = eosio::action_wrapper<"createpoollog"_n, &lucky::createpoollog>;
    using modifypoollog_action = eosio::action_wrapper<"modifypoollog"_n, &lucky::modifypoollog>;
    using delpoollog_action = eosio::action_wrapper<"delpoollog"_n, &lucky::delpoollog>;
    using uploadlog_action = eosio::action_wrapper<"uploadlog"_n, &lucky::uploadlog>;
    using takelog_action = eosio::action_wrapper<"takelog"_n, &lucky::takelog>;
    using drawlog_action = eosio::action_wrapper<"drawlog"_n, &lucky::drawlog>;
    using draw2log_action = eosio::action_wrapper<"draw2log"_n, &lucky::draw2log>;

   private:
    pool_table _pools = pool_table(_self, _self.value);
    static uint128_t combine_ids(const uint64_t& x, const uint64_t& y) { return (uint128_t{x} << 64) | y; }

    template <typename T>
    static void erase_all(T& table)
    {
        auto itr = table.begin();
        while (itr != table.end()) {
            itr = table.erase(itr);
        }
    }

    void take_num(const pool_row& p, const holder1_row& h);
    void transfer(const name from, const name to, const extended_asset value, const string& memo);
    config_row get_config();
    uint64_t get_pool_id();
    void require_auth_admin();

// DEBUG (used to help testing)
#ifdef DEBUG
    template <typename T>
    void clear_table(T& table, int64_t rows_to_clear);
#endif
};