#include "Order.h"

int Order::nextId = 6001;

Order::Order(Customer* cust) {
	buyer = cust;
	id = nextId++;
	price = 0.0;
}

Order::~Order() {
}

void Order::add(Product* prod) {
	purchArray.add(prod->getId());
	price += prod->getPrice();
}

PurchArray* Order::getPurch() {
	return &purchArray;
}

Customer* Order::getCust() const {
	return buyer;
}

int Order::getId() const {
	return id;
}

float Order::getPrice() {
	return price;
}

ostream& operator<<(ostream& out, Order& ord) {
	out << ord.getId() << "     " << ord.getCust()->getName() << "     " << ord.getPrice() << endl;
	out << *(ord.getPurch()) << endl;
	return out;
}