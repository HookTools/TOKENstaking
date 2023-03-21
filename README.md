# Token staking contract

- Token Staking is a smart contract that allows users to lock up their tokens and earn rewards for doing so. With the Hook Tools Token Staking contract, you can create your own staking contract and define the terms of the staking rewards.
- Users can stake their tokens and earn rewards, while you as the contract creator have the ability to set the staking rewards, define the staking period, and set the minimum and maximum amounts of tokens that can be staked.
- Once a user stakes their tokens, they will receive branded tokens or points that represent their staked tokens. These branded tokens can be used to redeem rewards or participate in other activities within your ecosystem.

`To use it you need to add **eosio.code** permission`


**TABLES:**

| Users  | Config |
| ------------- | ------------- |
| name username  | username  |
| stake[] | APY{} |




**stake[] =**

     {
    		uint64_t locktime;
            uint64_t starttime;
            asset token;
            double apy;
    }

**APY[] =**

	 {   
        uint64_t locktime;
        double apy;
    }
	
**ACTIONS:**

| | Claim  | Addapy |
|--| ------------- | ------------- |
|name  | username  | username  |
| uint64_t| id | |
| double | | apy |
