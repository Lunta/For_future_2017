from search_location import *
from info_map import *
from tkinter import *
from tkinter import ttk

# 등급	    좋음	보통	나쁨	매우나쁨
    # Grade 값	1	    2	    3	    4
    # 항목	    SO2	    CO	        O3	    NO2	         PM10	PM2.5
    # 이름      아황산  일산화탄소  오존    이산화질소   미세    초미세
    # 단위	    ppm	    ppm	        ppm	    ppm	         ㎍/㎥	㎍/㎥

window = Tk()
window.geometry("600x800+750+200")
window.title('대기오염정보 조회서비스')

info_map = InfoMap()
result_search = None
grade_dict = dict()
grade_dict[1] = '좋음'
grade_dict[2] = '보통'
grade_dict[3] = '나쁨'
grade_dict[4] = '매우나쁨'
color_dict = dict()
color_dict[1] = 'green'
color_dict[2] = 'yellow'
color_dict[3] = 'red'
color_dict[4] = 'black'
grade_kind = dict()
grade_kind['아황산'] = 'so2Grade'
grade_kind['일산화탄소'] = 'coGrade'
grade_kind['오존'] = 'o3Grade'
grade_kind['이산화질소'] = 'no2Grade'
grade_kind['미세먼지'] = 'pm10Grade'
grade_kind['초미세먼지'] = 'pm25Grade'
graph_title = None
canvas_height = 300
email_str = ''

def Search():
    global result_search, grade_dict, InfoNameList, color_dict
    result_search = SearchLocation(SearchEntry.get())
    result_search.lastest_info()
    locationName = Label(frame_search, text=result_search.id_location)
    locationName.place(x=170, y=10+ canvas_height)
    dataTime = Label(frame_search,     text=result_search.air_info[0]['dataTime'])
    dataTime.place(x=170, y=30+ canvas_height)
    so2Value = Label(frame_search,     text=str(result_search.air_info[0]['so2Value']) + 'ppm')
    so2Value.place(x=170, y=50+ canvas_height)
    coValue = Label(frame_search,      text=str(result_search.air_info[0]['coValue']) + 'ppm')
    coValue.place(x=170, y=70+ canvas_height)
    o3Value = Label(frame_search,      text=str(result_search.air_info[0]['o3Value']) + 'ppm')
    o3Value.place(x=170, y=90+ canvas_height)
    no2Value = Label(frame_search,     text=str(result_search.air_info[0]['no2Value']) + 'ppm')
    no2Value.place(x=170, y=110+ canvas_height)
    pm10Value = Label(frame_search,    text=str(result_search.air_info[0]['pm10Value']) + '㎍/㎥')
    pm10Value.place(x=170, y=130+ canvas_height)
    pm10Value24 = Label(frame_search,  text=str(result_search.air_info[0]['pm10Value24']) + '㎍/㎥')
    pm10Value24.place(x=170, y=150+ canvas_height)
    pm25Value = Label(frame_search,    text=str(result_search.air_info[0]['pm25Value']) + '㎍/㎥')
    pm25Value.place(x=170, y=170+ canvas_height)
    pm25Value24 = Label(frame_search,  text=str(result_search.air_info[0]['pm25Value24']) + '㎍/㎥')
    pm25Value24.place(x=170, y=190+ canvas_height)

    so2Grade = Label(frame_search, bg=color_dict[int(result_search.air_info[0]['so2Grade'])], text=grade_dict[int(result_search.air_info[0]['so2Grade'])])
    so2Grade.place(x=420, y=50+ canvas_height)
    coGrade = Label(frame_search,   bg=color_dict[int(result_search.air_info[0]['coGrade'])], text=grade_dict[int(result_search.air_info[0]['coGrade'])])
    coGrade.place(x=420, y=70+ canvas_height)
    o3Grade = Label(frame_search,   bg=color_dict[int(result_search.air_info[0]['o3Grade'])], text=grade_dict[int(result_search.air_info[0]['o3Grade'])])
    o3Grade.place(x=420, y=90+ canvas_height)
    no2Grade = Label(frame_search,  bg=color_dict[int(result_search.air_info[0]['no2Grade'])], text=grade_dict[int(result_search.air_info[0]['no2Grade'])])
    no2Grade.place(x=420, y=110+ canvas_height)
    pm10Grade = Label(frame_search, bg=color_dict[int(result_search.air_info[0]['pm10Grade'])], text=grade_dict[int(result_search.air_info[0]['pm10Grade'])])
    pm10Grade.place(x=420, y=130+ canvas_height)
    pm25Grade = Label(frame_search, bg=color_dict[int(result_search.air_info[0]['pm25Grade'])], text=grade_dict[int(result_search.air_info[0]['pm25Grade'])])
    pm25Grade.place(x=420, y=170+ canvas_height)

    InfoNameList.activate(5)

def UpdateGraph(event):
    global result_search, InfoNameList, graph_title, GraphCanvas, graph_bar, graph_label
    if result_search is None:
        return
    TAG = ['so2Value', 'coValue', 'o3Value', 'no2Value', 'pm10Value', 'pm25Value', 'dataTime']
    norm = dict()
    norm[TAG[0]] = 2000
    norm[TAG[1]] = 200
    norm[TAG[2]] = 2000
    norm[TAG[3]] = 2000
    norm[TAG[4]] = 2
    norm[TAG[5]] = 2
    if graph_title is None:
        graph_title = Label(frame_search, text='일간 ' + result_search.id_location + ' ' + InfoNameList.get(InfoNameList.curselection()[0]) + ' 정보')
    else:
        graph_title.configure(text='일간 ' + result_search.id_location + ' ' + InfoNameList.get(InfoNameList.curselection()[0]) + ' 정보')
    graph_title.pack()
    graph_title.place(x=160, y=0)
    idx = 0
    for bar in graph_bar:
        GraphCanvas.delete(bar)
    graph_bar.clear()
    label_idx = 0
    for info in result_search.air_info:
        if idx % 3 is 0:
            a = Label(frame_search, text=info['dataTime'].split(' ')[1])
            a.pack()
            a.place(x=425 - (idx * 20), y=canvas_height-30)
            try:
                vv = float(info[TAG[InfoNameList.curselection()[0]]])
                value = int(norm[TAG[InfoNameList.curselection()[0]]] * float(info[TAG[InfoNameList.curselection()[0]]]))
            except:
                vv = 0
                value = 0
            graph_label[label_idx].configure(text=vv)
            graph_label[label_idx].pack()
            graph_label[label_idx].place(x=(425 - (idx * 20)), y=245 - value)
            label_idx += 1
            rect = (435 - (idx * 20)), 265 - value, (445 - (idx * 20)), 265
            graph_bar.append(GraphCanvas.create_rectangle(rect, fill="red"))
        idx += 1

def UpdateMap(event):
    global mapLabel, info_map, grade_kind, mapSortList
    info_map.UpdateMapInfo(grade_kind[mapSortList.get(mapSortList.curselection()[0])])
    mapLabel.configure(image=info_map.map_png)
    mapLabel.image = info_map.map_png

def SortMapData():
    pass

tab = ttk.Notebook()
tab.pack()
tab.place(x=25, y=50)

frame_search = ttk.Frame(window, width=550, height=700, relief=SOLID)
frame_sort = ttk.Frame(window, width=550, height=700, relief=SOLID)
frame_map = ttk.Frame(window, width=550, height=700, relief=SOLID)
tab.add(frame_search, text='Search')
tab.add(frame_sort, text='Sort')
tab.add(frame_map, text='Map')

SearchLabel = Label(window, text="지역명 입력")
SearchLabel.place(x=150, y=20)
SearchEntry = Entry(window, bg='yellow')
SearchEntry.place(x=220, y=20)
SearchButton = Button(window, text="검색", command=Search)
SearchButton.place(x=365, y=16)
locationName = Label(frame_search, text='[지역명]                        :  ')
locationName.place(x=10, y=10 + canvas_height)
dataTime = Label(frame_search,     text='[일시]                           :  ')
dataTime.place(x=10, y=30 + canvas_height)
so2Value = Label(frame_search,     text='[아황산 농도]                 :  ')
so2Value.place(x=10, y=50 + canvas_height)
coValue = Label(frame_search,      text='[일산화탄소 농도]           :  ')
coValue.place(x=10, y=70 + canvas_height)
o3Value = Label(frame_search,      text='[오존 농도]                    :  ')
o3Value.place(x=10, y=90 + canvas_height)
no2Value = Label(frame_search,     text='[이산화질소 농도]           :  ')
no2Value.place(x=10, y=110 + canvas_height)
pm10Value = Label(frame_search,    text='[미세먼지 농도]              :  ')
pm10Value.place(x=10, y=130 + canvas_height)
pm10Value24 = Label(frame_search,  text='[미세먼지 일 평균 농도]   :  ')
pm10Value24.place(x=10, y=150 + canvas_height)
pm25Value = Label(frame_search,    text='[초미세먼지 농도]           :  ')
pm25Value.place(x=10, y=170 + canvas_height)
pm25Value24 = Label(frame_search,  text='[초미세먼지 일 평균 농도]:  ')
pm25Value24.place(x=10, y=190 + canvas_height)
so2Grade = Label(frame_search,  text='[아황산 지수]      :  ')
so2Grade.place(x=300, y=50 + canvas_height)
coGrade = Label(frame_search,   text='[일산화탄소 지수]:  ')
coGrade.place(x=300, y=70 + canvas_height)
o3Grade = Label(frame_search,   text='[오존 지수]         :  ')
o3Grade.place(x=300, y=90 + canvas_height)
no2Grade = Label(frame_search,  text='[이산화질소 지수]:  ')
no2Grade.place(x=300, y=110 + canvas_height)
pm10Grade = Label(frame_search, text='[미세먼지 지수]   :  ')
pm10Grade.place(x=300, y=130 + canvas_height)
pm25Grade = Label(frame_search, text='[초미세먼지 지수]:  ')
pm25Grade.place(x=300, y=170 + canvas_height)

GraphCanvas = Canvas(frame_search, bg="white", height=canvas_height - 30, width=550)
GraphCanvas.place(x=000, y=300)
GraphCanvas.pack()
graph_bar = []
graph_label = []
for i in range(8):
    tmp = Label(frame_search, text='')
    tmp.pack()
    tmp.place(x=0, y=0)
    graph_label.append(tmp)

coord = 10, 50, 240, 210
#arc = GraphCanvas.create_arc(coord, start=0, extent=150, fill="red")


mapLabel = Label(frame_map, image=info_map.map_png)
mapLabel.pack()
mapSortList = Listbox(frame_map, width=10, height=6)
mapSortList.insert(1, '아황산')
mapSortList.insert(2, '일산화탄소')
mapSortList.insert(3, '오존')
mapSortList.insert(4, '이산화질소')
mapSortList.insert(5, '미세먼지')
mapSortList.insert(6, '초미세먼지')
mapSortList.activate(5)
mapSortList.bind('<<ListboxSelect>>', UpdateMap)
mapSortList.pack()
mapSortList.place(x=0, y=0)

InfoNameList = Listbox(frame_search, width=10, height=6)
InfoNameList.insert(1, '아황산')
InfoNameList.insert(2, '일산화탄소')
InfoNameList.insert(3, '오존')
InfoNameList.insert(4, '이산화질소')
InfoNameList.insert(5, '미세먼지')
InfoNameList.insert(6, '초미세먼지')
InfoNameList.bind('<<ListboxSelect>>', UpdateGraph)
InfoNameList.pack()
InfoNameList.place(x=482, y=0) #canvas_height-100

window.mainloop()


