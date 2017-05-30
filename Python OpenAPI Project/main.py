from search_location import *
from info_map import *
from tkinter import *

#window = Tk()
#def process():
#    dollar = float(e1.get())
#    e2.insert(0, str(dollar*1200))
#l1 = Label(window, text="달러")
#l2 = Label(window, text="원")
#l1.grid(row=0, column=0)
#l2.grid(row=1, column=0)
#e1 = Entry(window, bg='yellow')
#e2 = Entry(window, bg='blue')
#e1.grid(row=0, column=1)
#e2.grid(row=1, column=1)
#b1 = Button(window, text="달러->원", command=process)
#b2 = Button(window, text="원->달러")
#b1.grid(row=2, column=0)
#b2.grid(row=2, column=1)
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

main()
#InfoMap()
