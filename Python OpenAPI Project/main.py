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


main()
#InfoMap()

def query_sido_data():
    str = 'http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/' \
              'getCtprvnRltmMesureDnsty?' \
              'sidoName='+ urllib.parse.quote("서울")+'&' \
              'pageNo=1&' \
              'numOfRows=10000&' \
              'ServiceKey=LkXZ0KCCJ4B52toUWxsA5xcobsnrNLOB55MHxbJbVBj1WUZ6P2BkN1Qka%2BytNClkxQbFC3mvvhxkI163p%2BTHjg%3D%3D&' \
              'ver=1.3'

    # 등급	    좋음	보통	나쁨	매우나쁨
    # Grade 값	1	    2	    3	    4
    # 항목	    SO2	    CO	        O3	    NO2	         PM10	PM2.5
    # 이름      아황산  일산화탄소  오존    이산화질소   미세    초미세
    # 단위	    ppm	    ppm	        ppm	    ppm	         ㎍/㎥	㎍/㎥

    request = urllib.request.Request(str)
    request.get_method = lambda: 'GET'
    response_body = urllib.request.urlopen(request).read().decode('utf-8')

    print (response_body)
    tags = ['stationName', 'dataTime', 'so2Value', 'coValue', 'o3Value', 'no2Value', 'pm10Value', 'pm10Value24',
            'pm25Value', 'pm25Value24', 'so2Grade', 'coGrade', 'o3Grade', 'no2Grade', 'pm10Grade', 'pm25Grade',
            'pm10Grade1H', 'pm25Grade1H']

    AtmosphereInfo = parseString(response_body)
    response = AtmosphereInfo.childNodes[0].childNodes
    body = response[3]
    item_list = body.childNodes[1].childNodes
    print(len(item_list))

    dic = {}
    day_info = []
    for item in item_list:
        if item.nodeName == "item":
            subitems = item.childNodes
            for atom in subitems:
                if atom.nodeName in tags:
                    if atom.firstChild is None:
                        dic[atom.nodeName] = 'None'
                    else:
                        dic[atom.nodeName] = atom.firstChild.nodeValue
            day_info.append(dict(dic))
