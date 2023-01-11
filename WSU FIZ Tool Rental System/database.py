import datetime
from tkinter import messagebox
from tinydb import TinyDB, Query

def createRentalsDB():
    return TinyDB('rentals.json')

def createItemsDB():
    return TinyDB('items.json')

def createRental(db, name):
    num = db.insert({'name': name, 'date': str(datetime.datetime.now())})
    db.update({'id': num}, doc_ids=[num])

def createItem(db, name):
    num = db.insert({'name': name, 'totalQTY': 1, 'loanedQTY': 0})
    db.update({'id': num}, doc_ids=[num])

def updateDataDB(db, id, data):
    db.update(data, doc_ids=[id])

def deleteDataDB(db, id):
    db.remove(doc_ids=[id])

def readWholeDB(db):
    return db.all()

def readDataDB(db, id):
    return db.get(doc_id=id)

def checkoutItem(rentalDB, itemDB, rentalID, itemID):
    #Read item data
    item = readDataDB(itemDB, itemID)
    #Take item out of item inventory if possible
    if item['totalQTY']-item['loanedQTY'] > 0:
        updateDataDB(itemDB, itemID, {'loanedQTY': item['loanedQTY']+1})
    else:
        return 0
    #Add item to rental or add qty if item already in rental
    newArray = []
    propertyDoesntExist = True
    if 'items' in readDataDB(rentalDB, rentalID):
        for i in readDataDB(rentalDB, rentalID)['items']:
            if i['id'] is itemID:
                newArray.append({'id':itemID, 'name': i['name'], 'qty': i['qty']+1})
                propertyDoesntExist = False
            else:
                newArray.append(i)
    
    if propertyDoesntExist:
        newArray.append({'id': itemID, 'name': item['name'], 'qty': 1})

    updateDataDB(rentalDB, rentalID, {'items':newArray})
    return 1


def checkinItem(rentalDB, itemDB, rentalID, itemID):
    #Read item data
    item = readDataDB(itemDB, itemID)
    #Put item back in item inventory if possible
    if item['loanedQTY'] > 0:
        updateDataDB(itemDB, itemID, {'loanedQTY': item['loanedQTY']-1})
    #Add item to rental or add qty if item already in rental
    newArray = []
    if 'items' in readDataDB(rentalDB, rentalID):
        for i in readDataDB(rentalDB, rentalID)['items']:
            if i['id'] is itemID:
                if i['qty']-1 > 0:
                    newArray.append({'id':itemID, 'name': i['name'], 'qty': i['qty']-1})
            else:
                newArray.append(i)
    else: 
        newArray.append({'id': itemID, 'name': item['name'], 'qty': 0})

    updateDataDB(rentalDB, rentalID, {'items':newArray})
    return 1

def idToVid(id):
    return str(id).rjust(12,'0')

#add try catch, add class name


rentals = createRentalsDB()
items = createItemsDB()

# print(readWholeDB(rentals))
# print(readWholeDB(items))




