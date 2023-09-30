# lucky.daobox

## Contracts

| name                                                  | description           |
| ----------------------------------------------------- | --------------------- |
| [lucky.daobox](https://bloks.io/account/lucky.daobox) | daobox lucky Contract |

## Quickstart

### `USER`

```bash
# holder take a number
$cleos push action lucky.daobox take '[1, "holder1"]' -p holder1
```

### `ADMIN`

```bash
# create pool
$cleos push action lucky.daobox createpool '[$current, $vote_end, $take_end, { sym: "4,EOS", contract: "eosio.token" }]' -p admin.daobox

# modify pool
$cleos push action lucky.daobox modifypool '[1, $current, $vote_end, $take_end, { sym: "4,EOS", contract: "eosio.token" }]' -p admin.daobox

# delete pool
$cleos push action lucky.daobox delpool '[1]' -p admin.daobox

# upload tickets
$cleos push action lucky.daobox upload '[1, [["holder1", 1, 3], ["holder2", 2, 9]]]' -p admin.daobox

# admin take a number
$cleos push action lucky.daobox take2 '[1, 50]' -p admin.daobox

# draw the winning number
$cleos push action lucky.daobox draw '[1]' -p admin.daobox

# draw the winning numbers for little award
$cleos push action lucky.daobox draw2 '[1]' -p admin.daobox
```

### Viewing Table Information

```bash
$cleos get table lucky.daobox lucky.daobox config
$cleos get table lucky.daobox lucky.daobox pools
$cleos get table lucky.daobox 1 holders1
$cleos get table lucky.daobox 1 holders1 -L holder1 -U holder1
$cleos get table lucky.daobox 1 holders2
$cleos get table lucky.daobox 1 holders2 -L holder1 -U holder1
```

## Table of Content

- [TABLE `config`](#table-config)
- [TABLE `pools`](#table-pools)
- [TABLE `holders1`](#table-holders1)
- [TABLE `holders2`](#table-holders2)
- [ACTION `init`](#action-updatestatus)
- [ACTION `createpool`](#action-createpool)
- [ACTION `modifypool`](#action-modifypool)
- [ACTION `delpool`](#action-delpool)
- [ACTION `upload`](#action-upload)
- [ACTION `take`](#action-take)
- [ACTION `take2`](#action-take2)
- [ACTION `draw`](#action-draw)
- [ACTION `draw2`](#action-draw2)
- [ACTION `createpoollog`](#action-createpoollog)
- [ACTION `modifypoollog`](#action-modifypoollog)
- [ACTION `delypoollog`](#action-delypoollog)
- [ACTION `uploadlog`](#action-uploadlog)
- [ACTION `takelog`](#action-takelog)
- [ACTION `drawlog`](#action-drawlog)
- [ACTION `draw2log`](#action-draw2log)

## TABLE `config`

### params

- `{uint64_t} pool_id` - last pool id
- `{uint64_t} fee_rate` - percentage of fees_account transferred(pips
  100/10000 of 1%)
- `{name} admin_account` - admin account
- `{name} fees_account` - receiving service charge account

### example

```json
{
  "pool_id": "2",
  "fee_rate": "10",
  "admin_account": "admin.daobox",
  "fees_account": "fees.daobox"
}
```

pools

## TABLE `pools`

### params

- `{uint64_t} id` - (primary key) reward pool id
- `{uint64_t} win_num` - win number
- `{uint64_t} winner` - prize-winner
- `{asset} rewards_quantity` - the rewards quantity
- `{uint64_t} vote_start` - voting start time
- `{uint64_t} vote_end` - voting end time
- `{uint64_t} take_end` - number pickup end time
- `{uint64_t} tickets` - total tickets
- `{extended_symbol} rewards` - rewards token
- `{uint64_t} last_take_num` - ratio of exchange

### example

```json
{
  "id": 2,
  "win_num": 0,
  "winner": "holder1",
  "rewards_quantity": "100.0000 EOS",
  "vote_start": "2022-12-30T02:30:00",
  "vote_end": "2022-12-30T02:33:00",
  "take_end": "2022-12-30T02:45:00",
  "tickets": 12,
  "rewards": {
    "sym": "4,EOS",
    "contract": "eosio.token"
  },
  "last_take_num": 9
}
```

## TABLE `holders1`

### params

- `{name} holder` - ticket holder
- `{uint64_t} sort` - sort number
- `{uint64_t} tickets` - the number of tickets that can be held

### example

```json
{
  "holder": "holder1",
  "sort": 2,
  "tickets": 9
}
```

## TABLE `holders2`

### params

- `{name} holder` - ticket holder
- `{uint64_t} sort` - sort number
- `{uint64_t} tickets` - the number of tickets that can be held
- `{uint64_t} from` - starting ticket number
- `{uint64_t} to` - end ticket number

### example

```json
{
  "holder": "holder1",
  "sort": 2,
  "tickets": 9,
  "from": 1,
  "to": 9
}
```

## ACTION `init`

> Initialize the contract

- **authority**: `admin.daobox`

### params

- `{uint16_t} fee_rate` - ratio of service charge
- `{name} admin_account` - lucky.daobox admin contract
- `{name} fees_account` - lucky.daobox fees contract

### Example

```bash
$ cleos push action lucky.daobox init '[10, "admin.daobox", "fees.daobox"]'
-p admin.daobox
```

## ACTION `createpool`

> Create pool

- **authority**: `admin.daobox`

### params

- `{time_point_sec} vote_start` - voting start time
- `{time_point_sec} vote_end` - voting end time
- `{time_point_sec} take_end` - number pickup end time
- `{extended_symbol} rewards` - rewards token

### Example

```bash
$ cleos push action lucky.daobox createpool '[1672281118, 1672885918,
1672972318, { sym: '4,EOS', contract: 'eosio.token' }]' -p admin.daobox
```

## ACTION `modifypool`

> Modify pool

- **authority**: `admin.daobox`

### params

- `{uint16_t} pool_id` - reward pool id
- `{time_point_sec} vote_start` - voting start time
- `{time_point_sec} vote_end` - voting end time
- `{time_point_sec} take_end` - number pickup end time
- `{extended_symbol} rewards` - rewards token

### Example

```bash
$ cleos push action lucky.daobox modifypool '[1, 1672281118, 1672885918,
1672972318, { sym: '4,EOS', contract: 'eosio.token' }]' -p admin.daobox
```

## ACTION `delpool`

> Delete pool

- **authority**: `admin.daobox`

### params

- `{uint64_t} pool_id` - reward pool id

### Example

```bash
$ cleos push action lucky.daobox delpool '[1]' -p lucky.daobox
```

## ACTION `upload`

> Initialize the rewards contract

- **authority**: `admin.daobox`

### params

- `{uint64_t} pool_id` - reward pool id
- `{vector<upload_item>} holder_tickets` - number of votes held by users

### Example

```bash
$ cleos push action lucky.daobox upload '[1, [ { holder: "account1",
tickets: 100, sort: 1 }]]' -p admin.daobox
```

## ACTION `take`

> Take number

- **authority**: `holder`

### params

- `{uint64_t} pool_id` - reward pool id
- `{name} holder` - ticket holder

### Example

```bash
$ cleos push action daobox take '[1, "holder1"]' -p holder1
```

## ACTION `take2`

> Take number

- **authority**: `admin.daobox`

### params

- `{uint64_t} pool_id` - reward pool id
- `{uint64_t} max_rows` - erase max row

### Example

```bash
$ cleos push action daobox take2 '[1, 50]' -p
admin.daobox
```

## ACTION `draw`

> Draw the winning numbers of a lottery

- **authority**: `admin.daobox`

### params

- `{uint64_t} pool_id` - reward pool id

### Example

```bash
$ cleos push action daobox draw '[1]' -p admin.daobox
```

## ACTION `draw2`

> Draw the winning numbers of a lottery

- **authority**: `admin.daobox`

### params

- `{uint64_t} pool_id` - reward pool id

### Example

```bash
$ cleos push action daobox draw2 '[1]' -p admin.daobox
```

## ACTION `recycle`

> Erase holder table data

- **authority**: `admin.daobox`

### params

- `{uint64_t} pool_id` - reward pool id
- `{uint64_t} max_rows` - erase max row

### Example

```bash
$ cleos push action daobox recycle '[1, 50]' -p admin.daobox
```

## ACTION `createpoollog`

> Generates a log when a pool is created.

- **authority**: `_self`

### params

- `{uint64_t} pool_id` - reward pool id
- `{time_point_sec} vote_start` - voting start time
- `{time_point_sec} vote_end` - voting end time
- `{time_point_sec} take_end` - number pickup end time
- `{extended_symbol} rewards` - rewards token

### example

```json
{
  "pool_id": 1,
  "vote_start": "2022-12-30T02:30:00",
  "vote_end": "2022-12-30T02:33:00",
  "take_end": "2022-12-30T02:45:00",
  "rewards": {
    "sym": "4,EOS",
    "contract": "eosio.token"
  }
}
```

## ACTION `modifypoollog`

> Generates a log when a pool is modify.

- **authority**: `_self`

### params

- `{uint64_t} pool_id` - reward pool id
- `{time_point_sec} vote_start` - voting start time
- `{time_point_sec} vote_end` - voting end time
- `{time_point_sec} take_end` - number pickup end time
- `{extended_symbol} rewards` - rewards token

### example

```json
{
  "pool_id": 1,
  "vote_start": "2022-12-30T02:30:00",
  "vote_end": "2022-12-30T02:33:00",
  "take_end": "2022-12-30T02:45:00",
  "rewards": {
    "sym": "4,EOS",
    "contract": "eosio.token"
  }
}
```

## ACTION `delpoollog`

> Generates a log when a pool is delete.

- **authority**: `_self`

### params

- `{uint64_t} pool_id` - reward pool id

### example

```json
{
  "pool_id": 1
}
```

## ACTION `uploadlog`

> Generates a log when upload ticket.

- **authority**: `_self`

### params

- `{uint64_t} pool_id` - reward pool id
- `{vector<upload_item>} holder_tickets` -

### example

```json
{
  "pool_id": "myprotocol",
  "holder_tickets": [
    {
      "holder": "holder1",
      "sort": 1,
      "tickets": 8
    },
    {
      "holder": "holder2",
      "sort": 2,
      "tickets": 9
    }
  ]
}
```

## ACTION `takelog`

> Generates a log when take a number.

- **authority**: `_self`

### params

- `{uint64_t} pool_id` - reward pool id
- `{name} holder` - ticket holder
- `{uint64_t} from` - starting ticket number
- `{uint64_t} to` - end ticket number

### example

```json
{
  "pool_id": 1,
  "holder": "holder1",
  "from": 10,
  "to": 100
}
```

## ACTION `drawlog`

> Generates a log when draw the winning numbers.

- **authority**: `_self`

### params

- `{uint64_t} pool_id` - reward pool id
- `{uint64_t} win_num` - winning number
- `{uint64_t} winner` - prize-winner
- `{asset} rewards_quantity` - the rewards quantity

### example

```json
{
  "pool_id": 1,
  "win_num": 99,
  "winner": "holder1",
  "rewards_quantity": "100.0000 EOS"
}
```
## ACTION `draw2log`

> Generates a log when draw the winning numbers.

- **authority**: `_self`

### params

- `{uint64_t} pool_id` - reward pool id
- `{vector<name>} winners` - winners
- `{asset} rewards_quantity` - the rewards quantity

### example

```json
{
  "pool_id": 1,
  "winners": [ "holder1" ],
  "rewards_quantity": "100.0000 EOS"
}
```