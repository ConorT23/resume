from asyncio.windows_events import NULL
from sys import maxsize
from tkinter import *
from tkinter.ttk import *
from database import *
from printer import *
from camera import *
from arduino import *
from tkinter import messagebox
import os.path
import ttkbootstrap as ttk
from ttkbootstrap.constants import *
import time


#Instantiate TK Library
window = ttk.Window(themename="lumen")
window.title("Tool Rental System")
icon = str(os.getcwd())+fr'\rentalsImg\wsuLogo.ico'
window.iconbitmap(icon)

#Window settings
#window.geometry("1100x500")
window.minsize(1200,600)
window.columnconfigure(7, minsize=200)

#expandability 
window.columnconfigure(0, weight=1)
window.columnconfigure(1, weight=1)
window.columnconfigure(2, weight=1)
window.columnconfigure(3, weight=1)
window.columnconfigure(4, weight=0)
window.columnconfigure(5, weight=1)
window.columnconfigure(6, weight=1)
window.columnconfigure(7, weight=2)
window.columnconfigure(8, weight=4)

window.rowconfigure(0, weight=0)
window.rowconfigure(1, weight=1)
window.rowconfigure(2, weight=1)
window.rowconfigure(3, weight=3)
window.rowconfigure(4, weight=0)


# for x in range(7):
#     window.columnconfigure(x, weight=1)

# for x in range(5):
#     window.rowconfigure(x, weight=1)

#global variables
scannerLoopRun = False
scannerAction = None
current_view = 'rental'
global selected_item , selected_item_2, selected_item_3
global selected_id, selected_id_2, selected_id_3
selected_item = None #rental & tool table selection
selected_id = None
selected_item_2 = None #manual checkout table selection
selected_id_2 = None
selected_item_3 = None #checked out tool selection
selected_id_3 = None

#Create Drop Down Menus
comPorts = listSerialPorts()
comPorts.insert(0,'Select')
camPorts = ['Select', 'Primary', 'Secondary']

#Selection Handlers for Dropdown
def comDropdownHandler(choice):
    global scanner_port
    scanner_port = openSerialPort(comVariable.get())
    print(scanner_port)

def camDropdownHandler(choice):
    choice = camVariable.get()
    print(choice)

def refreshBtnHandler():
    global comPorts, comDropdown
    global selected_id_3, selected_item_3

    if current_view == "tool":
        for i in rental_view.get_children():
            rental_view.delete(i)
        populate_items()
    elif current_view == "rental":
        for i in rental_view.get_children():
            rental_view.delete(i)
        populate_rentals()

    if(current_view == 'rental'):
        refresh_tools_checkedOut()


    comPorts = listSerialPorts()
    comDropdown["menu"].delete(0, "end")
    val = comVariable.get()
    comDropdown.grid_forget()
    # for value in range(len(comPorts)):
    #     comDropdown["menu"].add_command(label=comPorts[value], command=setComVar(comPorts[value]))
    comDropdown = OptionMenu(
    window,
    comVariable,
    *comPorts,
    bootstyle="primary-outline",
    command=comDropdownHandler
    )
    setComVar(val)
    comDropdown.grid(row = 0, column = 1, sticky = W)



def refresh_tools_checkedOut():
    global selected_id_3
    global selected_item_3
    if(selected_item is not None):
        data = readDataDB(rentals, selected_item[0])
        for i in tool_view.get_children():
            tool_view.delete(i)
        if 'items' in data:
            for element in data['items']:
                tool_view.insert('', 'end', values=(element['id'],str(element['qty'])+"x "+element['name']))
    else:
        for i in tool_view.get_children():
            tool_view.delete(i)
    # clear checked out tool table selection
    selected_id_3 = None
    selected_item_3 = None


def setComVar(value):
    global comVariable
    comVariable.set(value)

#Com Dropdown
comVariable = StringVar()
comDropdown = OptionMenu(
    window,
    comVariable,
    *comPorts,
    bootstyle="primary-outline",
    command=comDropdownHandler
)
comDropdown.grid(row = 0, column = 1, sticky = W)

comLabel = Label(window, text = "Scanner")
comLabel.grid(row = 0, column = 0, sticky = E, pady = 2, padx=(0,10))

#Cam Dropdown
camVariable = StringVar()
camDropdown = OptionMenu(
    window,
    camVariable,
    *camPorts,
    bootstyle="primary-outline",
    command=camDropdownHandler
)
camDropdown.grid(row = 0, column = 3, sticky = W)

camLabel = Label(window, text = "Camera")
camLabel.grid(row = 0, column = 2, sticky=E ,pady = 2, padx=(0,10))

#Refresh Btn
refreshBtn = Button(window, text = "Refresh", bootstyle="secondary", command=refreshBtnHandler)
refreshBtn.grid(row = 0, column = 4, sticky = W)

#scan serial for incoming data and call database checkout function based on returned ID
def executeScanCheckout():
    global scannerLoopRun
    global selected_id
    if scannerLoopRun:
        localID = readSerial(scanner_port)
        if localID is not None:
            print(localID)
            if(checkoutItem(rentals, items, selected_id, int(localID))):
                refresh_tools_checkedOut()
            else:
                item = readDataDB(items, int(localID))
                messagebox.showerror("Error", "Checkout Failed. No available {}s".format(item['name']))
        # Recursive Call
        window.after(500, executeScanCheckout)

# Allow for mode exit
def startStopCheckout():
    global scannerLoopRun, scannerAction
    scannerAction = 'checkout'
    if(selected_id is not None):
        try:
            if scannerLoopRun:
                writeSerial(scanner_port, "j")
                sideButton3.config(state='enabled')
                sideButton2.config(text='Check Out Item', bootstyle='default')
                scannerLoopRun = False
                print("Done")
            else: 
                writeSerial(scanner_port, "k")
                window.after(500, executeScanCheckout)
                sideButton3.config(state='disabled')
                sideButton2.config(text='Cancel', bootstyle='danger')
                scannerLoopRun= True
                print("Reading")
        except (NameError, TypeError, serial.SerialException, ValueError):
            messagebox.showerror('Error', 'Scanner not detected')
    else:
        messagebox.showerror('Error', 'Please select item')


#scan serial for incoming data and call database checkin function based on returned ID
def executeScanCheckin():
    global scannerLoopRun
    global selected_id
    if scannerLoopRun:
        localID = readSerial(scanner_port)
        if localID is not None:
            print(localID)
            checkinItem(rentals, items, selected_id, int(localID))
            refresh_tools_checkedOut()
        # Recursive Call
        window.after(500, executeScanCheckin)

# Allow for mode exit
def startStopCheckin():
    global scannerLoopRun, scannerAction
    scannerAction = 'checkin'
    if(selected_id is not None):
        try:
            if scannerLoopRun:
                writeSerial(scanner_port, "j")
                sideButton3.config(text='Check In Item', bootstyle='default')
                sideButton2.config(state='enabled')
                scannerLoopRun = False
                print("Done")
            else: 
                writeSerial(scanner_port, "k")
                window.after(500, executeScanCheckin)
                sideButton3.config(text='Cancel', bootstyle='danger')
                sideButton2.config(state='disabled')
                scannerLoopRun= True
                print("Reading")
        except (NameError, TypeError, serial.SerialException, ValueError):
            messagebox.showerror('Error', 'Scanner not detected')
    else:
        messagebox.showerror('Error', 'Please select item')

def toolsearchBtnHandler():
    text = toolSearchText.get()
    for i in checkout_table.get_children():
        checkout_table.delete(i)
    if text != '':
        for item in readWholeDB(items):
            if text.lower() in item['name'].lower():
                available = item['totalQTY'] - item['loanedQTY']
                checkout_table.insert('', 'end', values=(item['id'], item['name'], available))
    elif text == '':
        for item in readWholeDB(items):
            available = item['totalQTY'] - item['loanedQTY']
            checkout_table.insert('', 'end', values=(item['id'], item['name'], available))

def refresh_checkout_table():
        for i in checkout_table.get_children():
            checkout_table.delete(i)
        
        for item in readWholeDB(items):
            available = item['totalQTY'] - item['loanedQTY']
            checkout_table.insert('', 'end', values=(item['id'], item['name'], available))

def manual_checkin():
    if(selected_id_3 is not None):
        checkinItem(rentals, items, selected_id,selected_id_3)
        refresh_tools_checkedOut()
        messagebox.showinfo('Success', 'Check In Successful')
    else:
        messagebox.showerror('Error', 'Please select tool to check in')

def checkOut_selection_handler(event):
    index = checkout_table.selection()[0]
    global selected_item_2
    global selected_id_2
    selected_item_2 = checkout_table.item(index)['values']
    data = readDataDB(items, selected_item_2[0])
    selected_id_2 = data['id']

def manual_checkOut():
    if(selected_id_2 is not None):
        if(checkoutItem(rentals, items, selected_id, selected_id_2)):
            refresh_checkout_table()
            refresh_tools_checkedOut()
            messagebox.showinfo('Success', 'Check Out Successful')
        else:
            item = readDataDB(items, selected_id_2)
            messagebox.showerror("Error", "Checkout Failed. No available {}s".format(item['name']))
    else:
        messagebox.showerror('Error', 'Please select item')

def manual_checkOut_window():
    if(selected_id is not None):
        top=Toplevel(window)
        top.minsize(800,400)
        top.rowconfigure(0, weight=1, minsize=75)
        top.rowconfigure(1, weight=1)
        top.rowconfigure(2, weight=1)
        top.rowconfigure(3, weight=1, minsize=25)

        top.columnconfigure(0, weight=0, minsize=25)
        top.columnconfigure(1, weight=1)
        top.columnconfigure(2, weight=1)
        top.columnconfigure(3, weight=1)
        top.columnconfigure(4, weight=1, minsize=250)
        top.title("Manual Check Out")
        top.iconbitmap(icon)
        global checkout_table
        table_columns = ['ID', 'Name', '# Available']
        checkout_table = Treeview(top, columns=table_columns, height=20)
        checkout_table.grid(row = 1, column=1, columnspan=3, sticky='nsew')
        checkout_table.bind('<<TreeviewSelect>>', checkOut_selection_handler)

        #Scrollbar configuration
        scrollbar = Scrollbar(top, orient='vertical', bootstyle="default-round")
        scrollbar.configure(command=checkout_table.yview)
        scrollbar.grid(row = 1, column=1, columnspan=3 ,sticky='nse')
        checkout_table.config(yscrollcommand=scrollbar.set)

        checkout_table.column("#0",width=0)
        for col in table_columns:
            checkout_table.column(col)
            checkout_table.heading(col, text=col)

        checkoutButton = Button(top, width=20, text="Check Out Item", command=manual_checkOut)
        checkoutButton.grid(row=1, column=4, sticky=N)

        toolsearchLabel = Label(top, text = "Search")
        toolsearchLabel.grid(row = 0, column = 1, sticky = 'e', padx=(0,20))

        global toolSearchText
        toolSearchText = StringVar()
        toolsearchEntry = Entry(top, textvariable=toolSearchText, width=35)
        toolsearchEntry.grid(row = 0, column = 2, columnspan = 1, sticky='we')

        toolsearchBtn = Button(top, text = "Go", width = 10, command=toolsearchBtnHandler)
        toolsearchBtn.grid(row = 0, column = 3,  sticky = 'w', padx=(20,0))

        for item in readWholeDB(items):
            available = item['totalQTY'] - item['loanedQTY']
            checkout_table.insert('', 'end', values=(item['id'], item['name'], available))

    else:
        messagebox.showerror('Error', 'Please select item')


def addQTY():
    if(selected_item is not None):
        data = readDataDB(items, selected_item[0])
        data['totalQTY'] = data['totalQTY'] + 1
        updateDataDB(items, data['id'], data)
        sideEntry1.delete(0,END)
        sideEntry1.insert(0, data['totalQTY'])
        refreshBtnHandler()
    else:
        messagebox.showerror('Error', 'Please select item')

def subtractQTY():
    if(selected_item is not None):
        data = readDataDB(items, selected_item[0])
        if((data['totalQTY'] > 0) & (data['totalQTY'] > data['loanedQTY'])):
            data['totalQTY'] = data['totalQTY'] - 1
        updateDataDB(items, data['id'], data)
        sideEntry1.delete(0,END)
        sideEntry1.insert(0, data['totalQTY'])
        refreshBtnHandler()
    else:
        messagebox.showerror('Error', 'Please select item')

def write_tag():
    global scannerLoopRun, scannerAction
    scannerAction = "write_tag"
    if(selected_id is not None):
        try:
            if scannerLoopRun:
                writeSerial(scanner_port, "j")
                sideButton3.config(text='Write tag', bootstyle='default')
                #print('stopped')
                scannerLoopRun = False
            else: 
                writeSerial(scanner_port, "c")
                print("Writing")
                window.after(1500, write_tag_id)
                sideButton3.config(state='disable')
                sideButton3.config(text='Cancel', bootstyle='danger')
                scannerLoopRun = True
        except (NameError, TypeError, serial.SerialException):
            messagebox.showerror('Error', 'Scanner not connected')
    else:
        messagebox.showerror('Error', 'Please select item')


def write_tag_id():
    global selected_id
    if scannerLoopRun:
        writeSerial(scanner_port, idToVid(selected_id))
        print(idToVid(selected_id))
        time.sleep(2)
        sideButton3.config(state='enabled')
        checkDone()
    
def checkDone():
    global scanner_port, scannerLoopRun
    done = readSerial(scanner_port)
    if(done is not None):
        #print('done code: {}'.format(done))
        if(done == 'w'):
            print('Write success')
            write_tag()
        elif(done == 'f'):
            print('Write Failed')
            write_tag()
            messagebox.showerror('Error', 'Write Tag Failed')
        return
    elif scannerLoopRun == False:
        print('canceled')
        return
    window.after(500, checkDone)

def print_barcode():
    if(selected_id is not None):
        if current_view == "tool":
            generateBarcode(idToVid(selected_id))
        print("Barcode PDF Generated")
    else:
        messagebox.showerror('Error', 'Please select item')


def take_picture():
    if(selected_id is not None):
        takeImage(idToVid(selected_id), camVariable, current_view)
        print("Picture Saved")
    else:
        messagebox.showerror('Error', 'Please select item')

def delete_item():
    global selected_id, selected_id_3
    global selected_item, selected_item_3
    if(selected_id is not None):
        confirm = messagebox.askyesno("Confirm", "Are you sure you want to delete this selection?")
        if(confirm):
            if current_view == "tool":
                deleteDataDB(items, selected_id)
                print("Deleted")
                path = str(os.getcwd())+fr'\itemsImg\{idToVid(selected_id)}.png'
                if os.path.exists(path):
                    os.remove(path)
            elif current_view == "rental":
                data = readDataDB(rentals, selected_item[0])
                #check in all items currently checked out 
                if 'items' in data: 
                    for element in data['items']:
                        for i in range(element['qty']):
                            checkinItem(rentals, items, selected_id, element['id'])
                            #print("{} checked in".format(element['name']))

                path = str(os.getcwd())+fr'\rentalsImg\{idToVid(selected_id)}.png'
                if os.path.exists(path):
                    os.remove(path)
                deleteDataDB(rentals, selected_id)
                print("Deleted")

            sideLabel3.config(text='')
            sideEntry.delete(0,END)
            sideEntry1.delete(0,END)
            sideEntry2.delete(0,END)
            sideImage.config(image=img)
            selected_id = None
            selected_item = None
            selected_item_3 = None
            selected_id_3 = None
            refreshBtnHandler()
            messagebox.showwarning('Deleted', 'Selection Deleted. All tools that were checked out have been checked in.')
            ## need to check in all items currently checked out

    else:
        messagebox.showerror('Error', 'Please select item')

def save_changes():
    if(selected_item is not None):
        if current_view == "rental":
            data = readDataDB(rentals, selected_item[0])
            data['name'] = sideEntry_text.get()
            data['email'] = sideEntry2_text.get()
            updateDataDB(rentals, data['id'], data)
            messagebox.showinfo('Success', 'Changes saved')
            refreshBtnHandler()
        elif(current_view == "tool"):
            data = readDataDB(items, selected_item[0])
            data['name'] = sideEntry_text.get()
            updateDataDB(items, data['id'], data)
            for x in readWholeDB(rentals):
                if 'items' in x:
                    for item in x['items']:
                        if(item['id'] == data['id']):
                            item['name'] = data['name']
                            updateDataDB(rentals, x['id'], x)
            messagebox.showinfo('Success', 'Changes saved')
            refreshBtnHandler()
    else:  
        messagebox.showerror('Error', 'Please select item')


#Get items and insert into item view table
def populate_items():
    for item in readWholeDB(items):
        rental_view.insert('', 'end', values=(item['id'], item['name'], item['totalQTY']))


#Get rentals and insert into rental view table
def populate_rentals():
    for rental in readWholeDB(rentals):
        if(overdue(rental)):
            rental_view.insert('', 'end', values=(rental['id'], "{}  !".format(rental['name']), rental['date']))
        else:
            rental_view.insert('', 'end', values=(rental['id'], rental['name'], rental['date']))

def overdue(rental):
    current_date = datetime.datetime.now()
    rent_date = datetime.datetime.strptime(rental['date'], '%Y-%m-%d %H:%M:%S.%f')
    return_date = rent_date + datetime.timedelta(hours=48)
    if (current_date > return_date):
        #print('{} is overdue'.format(rental['name']))
        return 1
    else:
        return 0

def rentalBtnHandler():
    global current_view
    current_view = "rental"
    global selected_id, selected_item, selected_id_3, selected_item_3
    if(scannerLoopRun == True): #stop current scan
        if(scannerAction == 'checkin'):
            startStopCheckin()
        elif(scannerAction == 'checkout'):
            startStopCheckout() 
        elif(scannerAction == "write_tag"):
            write_tag()
    #clear selections
    if(selected_item is not None):
        selected_item = None
        selected_id = None
    if(selected_item_3 is not None):
        selected_item_3 = None
        selected_id_3 = None



    for i in tool_view.get_children():
        tool_view.delete(i)

    #update side window
    sideLabel1.config(text="Date")
    sideLabel2.config(text="Email")
    sideEntry.delete(0,END)
    sideEntry1.delete(0,END)
    sideEntry2.delete(0,END)
    sideLabel3.config(text='')
    sideImage.config(image=img)
    sideButton.grid_forget()
    sideButton7.grid_forget()
    sideButton2.config(text="Check Out Item", command=startStopCheckout)
    sideButton2.grid(row=3,column=7,sticky=N,pady=(260,0), padx=(25,0))
    sideButton3.config(text="Check In Item", command=startStopCheckin)
    sideButton3.grid(row=3, column=7, sticky=N,pady=(295,0), padx=(25,0))
    sideButton4.config(text="Delete", command=delete_item)
    sideButton4.grid(row=3, column=8, sticky=NE, pady=(325,0), padx=(0,50))
    sideButton5.config(text="Take Picture", command=take_picture)
    sideButton5.grid(row=3, column=8, sticky=N, pady=(175,0), padx=(0,50))
    sideButton6.config(text="Save Changes", command=save_changes)
    sideButton6.grid(row=3, column=7, sticky=N, pady=(330,0), padx=(25,0))
    sideButton8.config(text="Manual Check Out", command=manual_checkOut_window)
    sideButton8.grid(row=3, column=7, sticky=N, pady=(365,0), padx=(25,0))
    sideButton9.config(text="Manual Check In", command=manual_checkin)
    sideButton9.grid(row=3, column=7, sticky=N, pady=(400,0), padx=(25,0))
    tool_view.grid(row=3, column=7, sticky=N)
    toolScroll.grid(row=3, column=7, columnspan=1 ,sticky="nse", padx=(10,0))


    #refresh table 
    for i in rental_view.get_children():
       rental_view.delete(i)
    populate_rentals()
    

def toolBtnHandler():
    global current_view
    current_view = "tool"
    global selected_id, selected_item, selected_id_3, selected_item_3  
    if(scannerLoopRun == True): #stop current scan
        if(scannerAction == 'checkin'):
            startStopCheckin()
        elif(scannerAction == 'checkout'):
            startStopCheckout()
        elif(scannerAction == "write_tag"):
            write_tag()
    #clear selections
    if(selected_item is not None):
        selected_item = None
        selected_id = None
    if(selected_item_3 is not None):
        selected_item_3 = None
        selected_id_3 = None



    #update side window
    sideEntry.delete(0,END)
    sideEntry1.delete(0,END)
    sideEntry2.delete(0,END)
    sideLabel3.config(text='')
    sideImage.config(image=img)
    sideButton6.grid_forget()
    tool_view.grid_forget()
    toolScroll.grid_forget()
    sideButton8.grid_forget()
    #sideButton9.grid_forget()
    sideLabel1.config(text="Total")
    sideLabel2.config(text="Loaned")
    sideButton.config(text="+1 to QTY", command=addQTY)
    sideButton.grid(row=3,column=7,sticky=N,pady=(260,0), padx=(25,0))
    sideButton2.config(text="-1 to QTY", command=subtractQTY)
    sideButton2.grid(row=3, column=7, sticky=N,pady=(295,0), padx=(25,0))
    sideButton3.config(text="Write Tag", command=write_tag)
    sideButton3.grid(row=3, column=7, sticky=N, pady=(330,0), padx=(25,0))
    sideButton7.config(text="Print Barcode", command=print_barcode)
    sideButton7.grid(row=3, column=7, sticky=N, pady=(365,0), padx=(25,0))
    sideButton4.config(text="Delete", command=delete_item)
    sideButton4.grid(row=3, column=8, sticky=NE, pady=(325,0), padx=(0,50))
    sideButton5.config(text="Take Picture", command=take_picture)
    sideButton5.grid(row=3, column=8, sticky=N, pady=(175,0), padx=(0,50))
    sideButton9.config(text="Save Changes", command=save_changes)
    sideButton9.grid(row=3, column=7, sticky=N, pady=(400,0), padx=(25,0))

    #tool_view.grid(row=3, column=7)

    #refresh table
    for i in rental_view.get_children():
       rental_view.delete(i)
    populate_items()
    

#Rental and Tool Buttons
rentalBtn = Button(window, text = "Rentals", width=15 , command=rentalBtnHandler)
rentalBtn.grid(row = 1, column = 2, pady=10, padx=10, sticky="ws")

toolBtn = Button(window, text = "Tools", width=15 , command=toolBtnHandler)
toolBtn.grid(row = 1, column = 3, pady=10, padx=10, sticky="ws")

#creates popup window for adding rentals to rental database
def addRental_popup(): 
    top=Toplevel(window)
    top.title("Rental Entry Window")
    top.iconbitmap(icon)
    nameLabel = Label(top, text="Name: ").grid(row=0,column=0)
    global nameText
    nameText = StringVar()
    nameEntry = Entry(top, textvariable=nameText).grid(row=0,column=1)
    add_btn = Button(top, text="Add", bootstyle="success", command=lambda: add_rental(top)).grid(row=0, column=2, padx=10, pady=10)

# works with addRental_popup, takes entry and creates a rental in the database for that entry
def add_rental(top): 
    if nameText.get() == '':
        messagebox.showerror('Error', 'Please provide the required fields')
        return
    name = nameText.get()
    createRental(rentals, name)
    for i in rental_view.get_children():
       rental_view.delete(i)
    populate_rentals()
    top.destroy()

#creates popup window for adding items to item database
def addItem_popup(): 
    top=Toplevel(window)
    top.title("Tool Entry Window")
    top.iconbitmap(icon)
    nameLabel = Label(top, text="Name: ").grid(row=0,column=0)
    global nameText
    nameText = StringVar()
    nameEntry = Entry(top, textvariable=nameText).grid(row=0,column=1)
    add_btn = Button(top, text="Add", bootstyle="success", command=lambda: add_item(top)).grid(row=0, column=2, padx=10, pady=10)
    

# works with addItem_popup, takes entry and creates a item in the database for that entry
def add_item(top): 
    if nameText.get() == '':
        messagebox.showerror('Error', 'Please provide the required fields')
        return
    name = nameText.get()
    #qty = qtyText.get()
    createItem(items, name)
    for i in rental_view.get_children():
       rental_view.delete(i)
    populate_items()
    top.destroy()


def newBtnHandler():
    if current_view == "rental":
        addRental_popup()
    elif current_view == "tool":
        addItem_popup()
        

def searchBtnHandler():
    text = rentalSearchText.get()
    for i in rental_view.get_children():
        rental_view.delete(i)
    if text != '':
        if current_view == 'rental':
            for rental in readWholeDB(rentals):
                if (text.lower() in rental['name'].lower()) or (isinItems(rental, text)):
                    if(overdue(rental)):
                        rental_view.insert('', 'end', values=(rental['id'], "{}  !".format(rental['name']), rental['date']))
                    else:
                        rental_view.insert('', 'end', values=(rental['id'], rental['name'], rental['date']))
        elif current_view == 'tool':
                for item in readWholeDB(items):
                    if text.lower() in item['name'].lower():
                        rental_view.insert('', 'end', values=(item['id'], item['name'], item['totalQTY']))
    elif text == '':
        if current_view == 'rental':
                for rental in readWholeDB(rentals):
                    if(overdue(rental)):
                        rental_view.insert('', 'end', values=(rental['id'], "{}  !".format(rental['name']), rental['date']))
                    else:
                        rental_view.insert('', 'end', values=(rental['id'], rental['name'], rental['date']))
        elif current_view == 'tool':
                for item in readWholeDB(items):
                    rental_view.insert('', 'end', values=(item['id'], item['name'], item['totalQTY']))

def isinItems(rental, text):
    if 'items' in rental:
        for element in rental['items']:
            if text.lower() in element['name'].lower():
                return 1
    return 0


#Search and New
searchLabel = Label(window, text = "Search")
searchLabel.grid(row = 2, column = 0, sticky = 'se', pady = 2)

rentalSearchText = StringVar()
searchEntry = Entry(window, textvariable=rentalSearchText, width=35)
searchEntry.grid(row = 2, column = 1, columnspan = 2, pady = 2, padx=10, sticky='wes')

searchBtn = Button(window, text = "Go", width = 10, command=searchBtnHandler)
searchBtn.grid(row = 2, column = 3,  sticky = 'ws')

newBtn = Button(window, text = "New", width=10, command=newBtnHandler, bootstyle="success")
newBtn.grid(row = 2, column = 5, sticky = 'ws')

#table header styling
style = ttk.Style()
style.configure("Treeview.Heading", font=(None,12))

#Table List
# table_fr = Frame(window)
# table_fr.grid(row = 3, column=0, columnspan = 6, pady=10)
rental_columns = ['ID', 'Name', 'Date / QTY']
rental_view = Treeview(window, columns=rental_columns, height=20)
rental_view.grid(row = 3, column=0, columnspan = 6, pady=(10,20), sticky='nsew')

#Scrollbar configuration
rentScroll = Scrollbar(window, orient='vertical', bootstyle="default-round")
rentScroll.configure(command=rental_view.yview)
rentScroll.grid(row=3, column=0, columnspan = 6, sticky="nse", padx=9, pady=(0,20))
rental_view.config(yscrollcommand=rentScroll.set)

rental_view.column("#0",width=0)
for col in rental_columns:
    rental_view.column(col)
    rental_view.heading(col, text=col, command=lambda _col=col: \
                     treeview_sort_column(rental_view, _col, False))

def treeview_sort_column(tv, col, reverse):
    l = [(tv.set(k, col), k) for k in tv.get_children('')]
    l.sort(reverse=reverse)

    # rearrange items in sorted positions
    for index, (val, k) in enumerate(l):
        tv.move(k, '', index)

    # reverse sort next time
    tv.heading(col, command=lambda _col=col: treeview_sort_column(tv, _col, not reverse))


def display_rentalImage(data):
    vid = idToVid(data['id'])
    # path = fr'C:\Users\conor\Documents\SCHOOL\Tool Rental App\Senior-Design\rentalsImg\{vid}.png'
    path = str(os.getcwd())+fr'\rentalsImg\{vid}.png'
    if os.path.exists(path):
        toolimg = PhotoImage(file = path)
        img2 = toolimg.subsample(4,4)
    else:
        # toolimg = PhotoImage(file = r"C:\Users\conor\Documents\SCHOOL\Tool Rental App\Senior-Design\rentalsImg\wsuLogo.png")
        toolimg = PhotoImage(file = str(os.getcwd())+fr'\rentalsImg\wsuLogo.png')
        img2 = toolimg.subsample(18,18)       
    sideImage.config(image=img2)
    sideImage.image = img2 #keep reference


def display_toolImage(data):
    vid = idToVid(data['id'])
    # path = fr'C:\Users\conor\Documents\SCHOOL\Tool Rental App\Senior-Design\itemsImg\{vid}.png'
    path = str(os.getcwd())+fr'\itemsImg\{vid}.png'
    if os.path.exists(path):
        toolimg = PhotoImage(file = path)
        img2 = toolimg.subsample(4,4)
    else:
        # toolimg = PhotoImage(file = r"C:\Users\conor\Documents\SCHOOL\Tool Rental App\Senior-Design\rentalsImg\wsuLogo.png")
        toolimg = PhotoImage(file = str(os.getcwd())+fr'\rentalsImg\wsuLogo.png')
        img2 = toolimg.subsample(18,18)       
    sideImage.config(image=img2)
    sideImage.image = img2 #keep reference

#fills rhs tool table with checkout out items 
def update_toolTable(data):
    for i in tool_view.get_children():
        tool_view.delete(i)
    if 'items' in data:
        for element in data['items']:
            tool_view.insert('', 'end', values=(element['id'],str(element['qty'])+"x "+element['name']))


#fills side window with information of selected object in table
def selection_handler(event):
    index = rental_view.selection()[0]
    global selected_item, selected_id_3
    global selected_id, selected_item_3
    selected_item = rental_view.item(index)['values']
    sideEntry.delete(0,END)
    sideEntry1.delete(0,END)
    sideEntry2.delete(0,END)
    if current_view == "rental":
        data = readDataDB(rentals, selected_item[0])
        selected_id = data['id']
        vid = idToVid(data['id'])
        sideEntry.insert(0, data['name'])
        sideEntry1.insert(0, data['date'])
        sideLabel3.config(text=vid) 
        display_rentalImage(data)
        update_toolTable(data)
        if 'email' in data:
            sideEntry2.insert(0, data['email'])
    elif current_view == "tool":
        data = readDataDB(items, selected_item[0])
        selected_id = data['id']
        vid = idToVid(data['id'])
        sideEntry.insert(0, data['name'])
        sideEntry1.insert(0, data['totalQTY'])
        sideEntry2.insert(0, data['loanedQTY'])
        sideLabel3.config(text=vid)
        display_toolImage(data)
    #clear checked out tool selection
    selected_id_3 = None
    selected_item_3 = None
    if(scannerLoopRun == True): #stop current scan
        if(scannerAction == 'checkin'):
            startStopCheckin()
        elif(scannerAction == 'checkout'):
            startStopCheckout()
        elif(scannerAction == "write_tag"):
            write_tag()

def tool_view_selection_handler(event):
    index = tool_view.selection()[0]
    global selected_item_3
    global selected_id_3
    selected_item_3 = tool_view.item(index)['values']
    data = readDataDB(items, selected_item_3[0])
    selected_id_3 = data['id']



        
rental_view.grid(row=3, column=0, columnspan = 6, padx=25)
rental_view.bind('<<TreeviewSelect>>', selection_handler)



# side window init
sideEntry_text = StringVar()
sideEntry = Entry(window, textvariable=sideEntry_text, width=20)
sideEntry.grid(row = 3, column = 7, sticky = 'new', padx=(35,25))
sideLabel = Label(window, text="Name")
sideLabel.grid(row=3, column=6, sticky=NE)
sideEntry1_text = StringVar()
sideEntry1 = Entry(window, textvariable=sideEntry1_text, width=20)
sideEntry1.grid(row = 3, column = 7, sticky = 'new', pady=35, padx=(35,25))
sideLabel1 = Label(window)
sideLabel1.grid(row=3, column=6, sticky=NE, pady=35)
sideEntry2_text = StringVar()
sideEntry2 = Entry(window, textvariable=sideEntry2_text, width=20)
sideEntry2.grid(row = 3, column = 7, sticky = 'new', pady=70, padx=(35,25))
sideLabel2 = Label(window)
sideLabel2.grid(row=3, column=6, sticky=NE, pady=70)
sideLabel3 = Label(window)
sideLabel3.grid(row=3, column=8, sticky='n')
sideLabel4 = Label(window, text="ID :")
sideLabel4.grid(row=3, column=8, sticky=N, padx=(0,150))

img3 = PhotoImage(file = str(os.getcwd())+fr'\rentalsImg\trash.png')
trash_img = img3.subsample(50,50)

sideButton = Button(window, width=20)
sideButton2 = Button(window, width=20)
sideButton3 = Button(window, width=20)
sideButton4 = Button(window, image=trash_img, bootstyle="danger")
sideButton5 = Button(window, width=20)
sideButton6 = Button(window, width=20)
sideButton7 = Button(window, width=20)
sideButton8 = Button(window, width=20)
sideButton9 = Button(window, width=20)


#tool table for rentals
table2_fr = Frame(window)
table2_fr.grid(row = 3, column=7, columnspan=1, sticky=N, pady=(110,0), padx=(25,0))
tool_columns = ['Tool']
tool_view = Treeview(table2_fr, height=7)
tool_view['columns'] = ('id','Tool')
tool_view['displaycolumns'] = ('Tool')
tool_view.column("#0", width=0, stretch=NO)
tool_view.column("id", width=0, stretch=NO)
tool_view.column("Tool", anchor=CENTER, width=130)
tool_view.heading("Tool", text="Tools Out", anchor=CENTER)
toolScroll = Scrollbar(table2_fr, orient='vertical', bootstyle="default-round")
toolScroll.configure(command=tool_view.yview)
tool_view.config(yscroll=toolScroll.set)

#option menu approach for displaying rented tools
toolVariable = StringVar()
toolDropdown = OptionMenu(
    window,
    toolVariable
)
tool_view.bind('<<TreeviewSelect>>', tool_view_selection_handler)

#IMAGE INIT
#standard tk approach
# img1 = PhotoImage(file = r"C:\Users\conor\Documents\SCHOOL\Tool Rental App\Senior-Design\rentalsImg\wsuLogo.png")
img1 = PhotoImage(file = str(os.getcwd())+fr'\rentalsImg\wsuLogo.png')
img = img1.subsample(18,18)

sideImage = Label(window, image=img)
sideImage.image = img #keep reference of img 
sideImage.grid(row=3, column=8, sticky=N, pady=(35,0), padx=(0,50))


#Double Click Events
def onDoubleClick(event):
    item = rental_view.selection()
    for i in item:
        print("you clicked on", rental_view.item(i, "values")[0])

rental_view.bind("<Double-1>", onDoubleClick)

rentalBtnHandler()



  
# infinite loop which can be terminated by keyboard or mouse interrupt
window.mainloop()


