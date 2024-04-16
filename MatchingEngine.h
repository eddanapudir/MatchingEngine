#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <vector>
#include <unordered_map>

class GenericOrder
{
public:
	int order_id;
	int quantity;
	double price;
	std::string side;

	GenericOrder() = default;

	GenericOrder(int order_id, int quantity, double price, std::string side) :
		order_id{order_id}, quantity{quantity}, price{price}, side{side}
	{}
};

class NewOrder : public GenericOrder
{
public:
	NewOrder() = default;

	NewOrder(int order_id, int quantity, double price, std::string side) : 
		GenericOrder(order_id, quantity, price, side)
	{}
};

class ChangeOrder : public GenericOrder
{
public:
	ChangeOrder() = default;

	ChangeOrder(int order_id, int quantity, double price, std::string side) :
		GenericOrder(order_id, quantity, price, side)
	{}
};

class CancelOrder : public GenericOrder
{
public:
	CancelOrder() = default;

	CancelOrder(int order_id, int quantity, double price, std::string side) :
		GenericOrder(order_id, quantity, price, side)
	{}
};


class OrderBook
{
public:
	struct Order
	{
		int order_id;
		int quantity;
		double price;
		std::string side;

		Order() = default;

		Order(const Order&) = default;
		Order& operator=(const Order&) = default;
		Order(Order&&) = default;
		Order& operator=(Order&&) = default;


		Order(int orderId, int qty, double price, std::string side);
	};

	struct greaterComp
	{
		bool operator()(double val1, double val2) const
		{
			return val1 > val2;
		}
	};

	struct lessComp
	{
		bool operator()(double val1, double val2) const
		{
			return val1 < val2;
		}
	};

	OrderBook() = default;

	OrderBook(int orderId, int qty, double price, std::string side);

	void onSubmitOrder(GenericOrder&);
	void onChangeOrder(GenericOrder&);
	void onCancelOrder(GenericOrder&);
	void PrintOrderBook();
private:
	//Order order;

	void insert_into_container(std::map<double, std::pair<int, std::vector<Order>>>&, OrderBook::Order&);
	void update_container(std::map<double, std::pair<int, std::vector<Order>>>::iterator&, OrderBook::Order&);

	void update_multiple_records(OrderBook::Order&, std::map<double, std::pair<int, std::vector<Order>>>::iterator&);

	std::map<double, std::pair<int, std::vector<Order>>> bids;
	std::map<double, std::pair<int, std::vector<Order>>> asks;
	//std::map<double, std::pair<int, std::vector<Order>>, greaterComp*> bids;
	//std::map<double, std::pair<int, std::vector<Order>>, lessComp*> asks;
	
};