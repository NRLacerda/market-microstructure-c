
typedef struct 

int main(int argc, char *argv[]){
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
 - Hash Table
 - Linked List

 The hashtable will be used to store the orders by ID, so we can get an O(1) lookup when we have an event of type 
 Cancelled / Executed / 

 we must create the hash table before we can use it
 so we can store the orders with their ids, so when we get the cancelled event we just do an O(1) lookup to find the order
 and remove it.

 in the case of full execution we would need to remove it from the PriceLevel as well, for doing so we get the price, find the level it is
 and remove the amount from the total then we must iterate through the orders in that level to find the order, match the OrderId and then remove it from the queue

 theres some conditions here, if the order was fully executed we must change the "next" pointer of the previous order to the "next" of the current node
 if that was the only node of the PriceLevel, we must remove the price level from the book

 with that, each event will be processed and change how the orderbook (log) looks like, each message creates an new snapshot of the orderbook, with:
  	    1.) Ask Price 1: 	Level 1 Ask Price 	(Best Ask)
	    2.) Ask Size 1: 	Level 1 Ask Volume 	(Best Ask Volume)
	    3.) Bid Price 1: 	Level 1 Bid Price 	(Best Bid)
	    4.) Bid Size 1: 	Level 1 Bid Volume 	(Best Bid Volume)
	    5.) Ask Price 2: 	Level 2 Ask Price 	(2nd Best Ask)


 */


}