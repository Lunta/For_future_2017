from search_location import *
from info_map import *
from tkinter import *
from tkinter import ttk

window = Tk()
window.geometry("600x800+750+200")
window.title('대기오염정보 조회서비스')

tab = ttk.Notebook()
tab.pack()
tab.place(x=25, y=50)

frame_search = ttk.Frame(window, width=550, height=700, relief=SOLID)
frame_graph = ttk.Frame(window, width=550, height=700, relief=SOLID)
frame_map = ttk.Frame(window, width=550, height=700, relief=SOLID)
tab.add(frame_search, text='Search')
tab.add(frame_graph, text='Graph')
tab.add(frame_map, text='Map')


l1 = Label(window, text="달러")
l2 = Label(window, text="원")
l1.grid(row=0, column=0)
l2.grid(row=1, column=0)
e1 = Entry(window, bg='yellow')
e2 = Entry(window, bg='blue')
e1.grid(row=0, column=1)
e2.grid(row=1, column=1)
b1 = Button(window, text="달러->원")
b2 = Button(window, text="원->달러")
b1.grid(row=2, column=0)
b2.grid(row=2, column=1)
#window.mainloop()

def PrintMenu():
    print('')
    print('============Menu============')
    print('s - Search a location info')
    print('q - Quit')
    print('============================')


def main():
    run = True

    while run:
        PrintMenu()

        select = str(input('Select menu: '))

        if select is 's':
            result = SearchLocation()
            if result is not None:
                result.lastest_info()
        elif select is 'q':
            run = False


#main()
InfoMap()

