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

#include "InvControl.h"
#include "Store.h"

InvControl::InvControl() {
	initProducts();
	initCustomers();
}

void InvControl::launch(int argc, char* argv[]) {
	if (argc != 2) {
		view.printUsageError();
		return;
	}

	string option(argv[1]);

	if (option == "-a") {
		processAdmin();
	} else if (option == "-c") {
		processCashier();
	} else {
		view.printUsageError();
		return;
	}
}

void InvControl::processAdmin() {
	int choice;
	string name, size;
	int units, id;
	float price;

	while (1) {
		view.adminMenu(choice);
		switch(choice) {
			Product* prod;
			case 1: // ADD PRODUCT
				view.promptForStr("Product name", name);
				view.promptForStr("Product size", size);
				view.promptForInt("# units", units);
				view.promptForFloat("Price", price);
				prod = new Product(name, size, units, price);
				store.addProd(prod);
				view.pause();
				break;
			case 2: // ADD INVENTORY
				while(1) {
					view.promptForInt("Enter the product id [0 to exit]: ", id);
					if(id == 0)
						break;
					view.promptForInt("Enter the number of units: ", units);

					// a Store::addStock(id, units) would be better
					if(store.getStock().find(id, &prod)) {
						prod->setUnits(prod->getUnits() + units);		
					} else {
						view.printError("That product does not exist.");
					}
				}
				break;
			case 3: // PRINT INVENTORY
				view.printStock(store.getStock());
				view.pause();
				break;
			case 4: // PRINT CUSTOMERS
				view.printCustomers(store.getCustomers());
				view.pause();
				break;
			case 5: // REMOVE PRODUCT
				view.promptForInt("Enter the ID of a product to remove: ", id);

				if(store.getStock().find(id, &prod)) {
					store.getStock().remove(prod);
				} else {
					view.printError("That Product does not exist.");
				}
				break;
			case 6: // PRINT ORDERS
				{
					OrderArray ord;
					orderServer.retrieve(ord);
					view.printOrders(ord);
					view.pause();
				}
				break;
			default:
				return;
		}
	}
}

void InvControl::processCashier() {
	int choice;
	int prodId, custId;

	while (1) {
		view.cashierMenu(choice);
		switch(choice) {
			case 1: // PRODUCT PURCHASES
				Customer* cust;
				Product* prod;
				Order* order;
				while(true) {
					view.promptForInt("Please enter the customer id", custId);

					if(store.getCustomers().getById(custId, &cust))
						break;
					else
						view.printError("This customer does not exist.");
				}

				order = new Order(cust);

				while(1) {
					view.promptForInt("Please enter the product ids [0 to exit]", prodId);
					if(prodId == 0)
						break;

					if(!store.getStock().find(prodId, &prod)) {
						view.printError("That product does not exist.");
						continue;
					}

					if(prod->getUnits() < 1) {
						view.printError("That product is out of stock.");
						continue;
					}
					store.purchase(cust, prod);
					order->add(prod);
				}
				orderServer.update(order);
				break;
			case 2: // PRODUCT RETURNS
				view.printError("Feature not implemented");
				break;
			case 42: // PRINT STOCK & CUSTOMERS
				view.printStock(store.getStock());
				view.printCustomers(store.getCustomers());
				//
				{
					OrderArray ord;
					orderServer.retrieve(ord);
					view.printOrders(ord);
				//
				}
				view.pause();
				break;
			default:
				return;
		}
	}
}

void InvControl::initProducts() {
	store.addProd(new Product("Peachy Laundry Soap", "2 L", 3, 8.99f));
	store.addProd(new Product("Daisy's Spicy Chili", "150 g", 5, 1.29f));
	store.addProd(new Product("Daisy's Maple Baked Beans", "220 g", 2, 2.49f));
	store.addProd(new Product("Marmaduke Hot Dogs", "12-pack", 4, 4.99f));
	store.addProd(new Product("Garfield Hamburger Patties", "900 g", 2, 11.99f));
	store.addProd(new Product("Chunky Munkey Ice Cream", "2 L", 1, 2.97f));
	store.addProd(new Product("It's Your Bday Chocolate Cake", "500 g", 0, 12.99f));
	store.addProd(new Product("Happy Baker's Hot dog buns", "12-pack", 5, 3.49f));
	store.addProd(new Product("Happy Baker's Hamburger buns", "8-pack", 8, 3.99f));
	store.addProd(new Product("Moo-cow 2% milk", "1 L", 7, 2.99f));
	store.addProd(new Product("Moo-cow 2% milk", "4 L", 3, 4.99f));
	store.addProd(new Product("Moo-cow 5% coffee cream", "250 ml", 4, 1.49f));
	store.addProd(new Product("Good Morning Granola Cereal", "400 g", 2, 5.49f));
	store.addProd(new Product("Lightening Bolt Instant Coffee", "150 g", 8, 4.99f));
	store.addProd(new Product("Lightening Bolt Decaf Coffee", "100 g", 2, 4.99f));
	store.addProd(new Product("Munchies BBQ Chips", "250 g", 7, 2.99f));
	store.addProd(new Product("Munchies Ketchup Chips", "250 g", 3, 2.99f));
	store.addProd(new Product("Dogbert Salted Chips", "210 g", 4, 1.99f));
	store.addProd(new Product("Dogbert Sweet and Spicy Popcorn", "180 g", 5, 2.29f));
	store.addProd(new Product("Sudzzy Dish Soap", "1 L", 10, 3.99f));
}

void InvControl::initCustomers() {
	store.addCust(new Customer("Starbuck"));
	store.addCust(new Customer("Apollo"));
	store.addCust(new Customer("Boomer"));
	store.addCust(new Customer("Athena"));
	store.addCust(new Customer("Helo"));
	store.addCust(new Customer("Crashdown"));
	store.addCust(new Customer("Hotdog"));
	store.addCust(new Customer("Kat"));
	store.addCust(new Customer("Chuckles"));
	store.addCust(new Customer("Racetrack"));
}