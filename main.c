 /* 
 The Book has two sides, ASK (sell) and BID (buy), that's specified on the direction that can be 1 for BID and -1 for ASK.

 Beyond the direction we also have the type of it, that can be:
 Type	Meaning
    1	New limit order
    2	Partial cancellation
    3	Full deletion
    4	Execution (visible)
    5	Execution (hidden)

 We must create two structures:
 1 Hash Table
 2 Linked List

 1 - The hashtable will be used to store the orders by ID, so we can get an O(1) lookup
 
 2 - The linked list will be used to store the orders by price level, each level will have the total amount of orders and a pointer to the next 
 level, so we can iterate through the levels to get the best bid and ask, the linked list is an O(n) lookup.

 In the case of full execution/deletion we would need to remove it 
 
 This must be done in an exact order:

 1) Remove from price level 
 2) Remove from hash table
 3) free(order)

 1 - WIth the orderId in hands, find it at the hash table, get the price of it, with the price iterate through the price level to find the order, 
 remove it changing the "next" pointer of the previous order to the next of the current node.

 2 - WIth the orderId again, take the pointer of it, remove it from the hash table

 3 - With the pointer from the previous step now do an free(order) to release the memory

 With each message-event processed it will change how the orderbook looks like, each message creates an new snapshot of the orderbook, with:
  	    1.) Ask Price 1: 	Level 1 Ask Price 	(Best Ask)
	    2.) Ask Size 1: 	Level 1 Ask Volume 	(Best Ask Volume)
	    3.) Bid Price 1: 	Level 1 Bid Price 	(Best Bid)
	    4.) Bid Size 1: 	Level 1 Bid Volume 	(Best Bid Volume)
	    5.) Ask Price 2: 	Level 2 Ask Price 	(2nd Best Ask)


        Message → OrderBook allocates Order
        → Insert into structures
        → Later cancel/match
        → OrderBook frees Order


    

 */

int main(int argc, char *argv[]){
    

    return 0;
}