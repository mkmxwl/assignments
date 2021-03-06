/* * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                 */
/*  Program:  Simple Inventory System              */
/*  Author:   Christine Laurendeau                 */
/*  Date:     28-JUN-2016                          */
/*                                                 */
/*  (c) 2016 Christine Laurendeau                  */
/*  All rights reserved.  Distribution and         */
/*  reposting, in part or in whole, without the    */
/*  written consent of the author, is illegal.     */
/*                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * */

/*
	Represents a store.
*/

#ifndef STORE_H
#define STORE_H

#include "ProdArray.h"
#include "Product.h"
#include "CustArray.h"
#include "Customer.h"

class Store {
	public:
		~Store();
		void addProd(Product*);
		void addCust(Customer*);
		ProdArray& getStock();
		CustArray& getCustomers();
		void purchase(Customer*, Product*);
	private:
		ProdArray stock;
		CustArray customers;
};

#endif