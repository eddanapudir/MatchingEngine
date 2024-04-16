// MatchEngineApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "MatchingEngine.h"
#include <utility>
#include <algorithm>


void OrderBook::insert_into_container(std::map<double, std::pair<int, std::vector<Order>>>& container, OrderBook::Order& rec)
{
    //No entry there in container pending to be executed. Make a new entry
    std::cout << "No entry there in container pending to be executed. Make a new entry" << std::endl;
    std::vector<Order> temp;
    std::cout << rec.order_id << " : " << rec.price << " : " << rec.quantity << " : " << rec.side << std::endl;
    temp.push_back(rec);
    container[rec.price] = std::make_pair(rec.quantity, temp);
}

void OrderBook::update_container(std::map<double, std::pair<int, std::vector<Order>>>::iterator& itr, OrderBook::Order& rec)
{
    std::cout << "Entry exists - add to that quantity" << std::endl;
    auto existing_qty = itr->second.first;
    auto updated_qty = existing_qty + rec.quantity;
    itr->second.first = updated_qty;
    itr->second.second.push_back(rec);
}

void OrderBook::update_multiple_records(OrderBook::Order& rec, std::map<double, std::pair<int, std::vector<Order>>>::iterator& itrSide)
{
    std::cout << rec.order_id << " : " << rec.price << " : " << rec.quantity << " : " << rec.side << std::endl;

    auto existing_qty = itrSide->second.first;
    std::cout << "existing_qty : " << existing_qty << std::endl;
    auto updated_qty = existing_qty - rec.quantity;

    std::cout << "update_qty : " << updated_qty << std::endl;

    itrSide->second.first = updated_qty;
    auto& recVec = itrSide->second.second;

    auto tempQty = rec.quantity;
    auto itr = recVec.begin();

    while ((itr != recVec.end()) && (tempQty > 0))
    {
        if (itr->quantity == tempQty)
        {
            itr = recVec.erase(itr);
            tempQty = 0;
        }
        else if (itr->quantity < tempQty)
        {
            tempQty = tempQty - itr->quantity;
            itr = recVec.erase(itr);
        }
        else
        {
            itr->quantity -= tempQty;
            tempQty = 0;
        }
    }
}

void OrderBook::onSubmitOrder(GenericOrder& order)
{
    OrderBook::Order rec(order.order_id, order.quantity, order.price, order.side);
    if (rec.side == std::string("BUY"))
    {
        auto itrAsks = asks.find(rec.price);
        using Vec = std::vector<Order>;

        if(itrAsks == asks.end())
        {
            //There is No SELL order means there is no seller willing to sell at that buyer's price
            //So insert that record in bids container
            
            //Here again you check in bids container to see if there is a BUY order pending to be executed 
            //with that price.
            std::cout << "No entry in asks with matching price. So there is no seller willing to sell at that buyer's price" << std::endl;
            auto itrBids = bids.find(rec.price);

            if (itrBids == bids.end())
            {
                //No entry there in bids pending to be executed. Make a new entry
                insert_into_container(bids, rec);
            }
            else
            {
                //Entry exists - add to that quantity
                update_container(itrBids, rec);
            }
        }
        else
        {
            //There exists a seller willing to sell at that buyer's price
            //Check the quantity and perform modification.
            std::cout << "There exists a seller willing to sell at that buyer's price" << std::endl;
            if (rec.quantity == itrAsks->second.first)
            {
                //If BUY quantity is equal to SELL quantity - remove that entry from SELLer's record
                std::cout << "If BUY quantity is equal to SELL quantity - remove that entry from SELLer's record" << std::endl;
                asks.erase(itrAsks->first);
            }
            else if (rec.quantity < itrAsks->second.first)
            {
                //If BUY quantity is less
                std::cout << "If BUY quantity is less" << std::endl;
                std::cout << rec.order_id << " : " << rec.price << " : " << rec.quantity << " : " << rec.side << std::endl;

                update_multiple_records(rec, itrAsks);
            }
            else
            {
                //If BUY quantity is more
                std::cout << "If BUY quantity is more" << std::endl;
                auto existing_qty = itrAsks->second.first;
                auto remaining_qty_to_buy = rec.quantity - existing_qty;
                asks.erase(itrAsks->first);
                rec.quantity = remaining_qty_to_buy;

                //Remaining quantity to buy will go in bids container
                auto temp = Vec{ rec };
                bids[remaining_qty_to_buy] = std::make_pair(remaining_qty_to_buy, temp);
            }
        }
    }
    else if(rec.side == "SELL")
    {
        auto itrBids = bids.find(rec.price);
        using Vec = std::vector<Order>;
 
        if (itrBids == bids.end())
        {
            //There is No BUY order means there is no buyer willing to buy at that seller's price
            //So insert that record in asks container

            //Here again you check in asks container to see if there is a SELL order pending to be executed 
            //with that price.

            std::cout << "No entry in bids with matching price. So there is no buyer willing to buy at that seller's price" << std::endl;
            auto itrAsks = asks.find(rec.price);

            if (itrAsks == asks.end())
            {
                //No entry there in asks pending to be executed. Make a new entry
                insert_into_container(asks, rec);
            }
            else
            {
                //Entry exists - add to that quantity
                update_container(itrAsks, rec);
            }
        }
        else
        {
            //There exists a buyer willing to buy at that seller's price
            //Check the quantity and perform modification.
            std::cout << "There exists a buyer willing to buy at that seller's price" << std::endl;

            if (rec.quantity == itrBids->second.first)
            {
                //If SELL quantity is equal to BUY quantity - remove that entry from BUYer's record
                std::cout << "If BUY quantity is equal to SELL quantity - remove that entry from BUYer's record" << std::endl;
                bids.erase(itrBids->first);
            }
            else if (rec.quantity < itrBids->second.first)
            {
                //If SELL quantity is less
                std::cout << "If SELL quantity is less" << std::endl;
                std::cout << rec.order_id << " : " << rec.price << " : " << rec.quantity << " : " << rec.side << std::endl;

                update_multiple_records(rec, itrBids);
            }
            else
            {
                //If SELL quantity is more
                std::cout << "If SELL quantity is more" << std::endl;
                auto existing_qty = itrBids->second.first;
                auto remaining_qty_to_buy = rec.quantity - existing_qty;
                bids.erase(itrBids->first);
                rec.quantity = remaining_qty_to_buy;

                //Remaining quantity to sell will go in asks container
                auto temp = Vec{ rec };
                asks[remaining_qty_to_buy] = std::make_pair(remaining_qty_to_buy, temp);
            }
        }
    }
}

OrderBook::Order::Order(int orderId, int qty, double price, std::string side)
    :order_id(orderId), quantity(qty), price(price), side(side)
{
    std::cout << "Called here" << std::endl;
    std::cout << "I am here in constructor " << side << std::endl;
}

OrderBook::OrderBook(int orderId, int qty, double price, std::string side) //: order(orderId, qty, price, side)
{}

void OrderBook::onChangeOrder(GenericOrder&)
{}

void OrderBook::onCancelOrder(GenericOrder&)
{}

void OrderBook::PrintOrderBook()
{
    std::cout << "Printing entries in BIDS ....\n";

    for (const auto& each : bids)
    {
        auto price = each.first;
        auto qty_pair = each.second;

        std::cout << " Price : " << price << ", Total Quantity : " << qty_pair.first << std::endl;

        auto recs = qty_pair.second;
        std::cout << "............................................................................................................................... Orders : \n";

        for (const auto& rec : recs)
        {
            std::cout << "......................... : " << rec.order_id << ", " << rec.price << ", " << rec.quantity << ", " << rec.side << std::endl;
        }

        std::cout << "............................................................................................................................... End Of Orders : \n";
    }

    std::cout << "Printing entries in ASKS ....\n";
    for (const auto& each : asks)
    {
        auto price = each.first;
        auto qty_pair = each.second;

        std::cout << " Price : " << price << ", Total Quantity : " << qty_pair.first << std::endl;

        auto recs = qty_pair.second;
        std::cout << "............................................................................................................................... Orders : \n";

        for (const auto& rec : recs)
        {
            std::cout << "......................... : " << rec.order_id << ", " << rec.price << ", " << rec.quantity << ", " << rec.side << std::endl;
        }
        std::cout << "............................................................................................................................... End Of Orders : \n";
    }
}

void invokeOrderType(OrderBook& obj, std::string orderType, int order_id, int quantity, double price, std::string side)
{
    GenericOrder* order;
    if (orderType == "New")
    {
        order = new NewOrder(order_id, quantity, price, side);
        obj.onSubmitOrder(*order);
    }
    else if (orderType == "Change")
    {
        order = new ChangeOrder();
        obj.onChangeOrder(*order);
    }
    else if (orderType == "Cancel")
    {
        order = new CancelOrder();
        obj.onCancelOrder(*order);
    }
}

int main()
{
    OrderBook obj;
   
    invokeOrderType(obj, "New", 1, 100, 150.01, "SELL");
   // invokeOrderType(obj, "New", 2, 50, 150.01, "SELL");
   // invokeOrderType(obj, "New", 1, 85, 150.01, "SELL");

    invokeOrderType(obj, "New", 2, 78, 149.11, "SELL");
    invokeOrderType(obj, "New", 2, 20, 147.82, "SELL");

    obj.PrintOrderBook();

    invokeOrderType(obj, "New", 1, 50, 150.01, "BUY");

    /*invokeOrderType(obj, "New", 3, 80, 147.69, "BUY");
    invokeOrderType(obj, "New", 4, 65, 147.69, "BUY");
    invokeOrderType(obj, "New", 5, 100, 150.24, "BUY");*/

    /*invokeOrderType(obj, "New", 1, 45, 147.69, "SELL");
    invokeOrderType(obj, "New", 2, 100, 147.69, "SELL");
    obj.PrintOrderBook();

    invokeOrderType(obj, "New", 2, 185, 147.69, "BUY");
    obj.PrintOrderBook();*/

    //invokeOrderType(obj, "New", 5, 45, 147.69, "SELL");
    /*invokeOrderType(obj, "New", 6, 65, 132.19, "SELL");
    invokeOrderType(obj, "New", 7, 100, 142.56, "SELL");
    invokeOrderType(obj, "New", 8, 162, 145.49, "SELL");
    invokeOrderType(obj, "New", 9, 65, 146.04, "SELL");
    invokeOrderType(obj, "New", 10, 100, 144.87, "SELL");*/

    //invokeOrderType(obj, "New", 001, 100, 150.02, "SELL");

    std::cout << "After all transactions" << std::endl;
    obj.PrintOrderBook();

    return 0;
}