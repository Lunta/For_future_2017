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


SearchLabel = Label(window, text="지역명 입력")
SearchLabel.place(x=150, y=20)
SearchEntry = Entry(window, bg='yellow')
SearchEntry.place(x=220, y=20)
SearchButton = Button(window, text="검색")
SearchButton.place(x=365, y=16)
window.mainloop()

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
#InfoMap()

