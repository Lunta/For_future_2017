from search_location import *
from info_map import *
from tkinter import *
from tkinter import ttk

# 등급	    좋음	보통	나쁨	매우나쁨
    # Grade 값	1	    2	    3	    4
    # 항목	    SO2	    CO	        O3	    NO2	         PM10	PM2.5
    # 이름      아황산  일산화탄소  오존    이산화질소   미세    초미세
    # 단위	    ppm	    ppm	        ppm	    ppm	         ㎍/㎥	㎍/㎥

info_map = InfoMap()
result_search = None
grade_dict = dict()
grade_dict[1] = '좋음'
grade_dict[2] = '보통'
grade_dict[3] = '나쁨'
grade_dict[4] = '매우나쁨'

def Search():
    global result_search, grade_dict

    result_search = SearchLocation(SearchEntry.get())
    result_search.lastest_info()
    locationName = Label(frame_search, text=result_search.id_location)
    locationName.place(x=170, y=10)
    dataTime = Label(frame_search,     text=result_search.air_info[0]['dataTime'])
    dataTime.place(x=170, y=30)
    so2Value = Label(frame_search,     text=str(result_search.air_info[0]['so2Value']) + 'ppm')
    so2Value.place(x=170, y=50)
    coValue = Label(frame_search,      text=str(result_search.air_info[0]['coValue']) + 'ppm')
    coValue.place(x=170, y=70)
    o3Value = Label(frame_search,      text=str(result_search.air_info[0]['o3Value']) + 'ppm')
    o3Value.place(x=170, y=90)
    no2Value = Label(frame_search,     text=str(result_search.air_info[0]['no2Value']) + 'ppm')
    no2Value.place(x=170, y=110)
    pm10Value = Label(frame_search,    text=str(result_search.air_info[0]['pm10Value']) + '㎍/㎥')
    pm10Value.place(x=170, y=130)
    pm10Value24 = Label(frame_search,  text=str(result_search.air_info[0]['pm10Value24']) + '㎍/㎥')
    pm10Value24.place(x=170, y=150)
    pm25Value = Label(frame_search,    text=str(result_search.air_info[0]['pm25Value']) + '㎍/㎥')
    pm25Value.place(x=170, y=170)
    pm25Value24 = Label(frame_search,  text=str(result_search.air_info[0]['pm25Value24']) + '㎍/㎥')
    pm25Value24.place(x=170, y=190)

    so2Grade = Label(frame_search,  text=grade_dict[int(result_search.air_info[0]['so2Grade'])])
    so2Grade.place(x=420, y=50)
    coGrade = Label(frame_search,   text=grade_dict[int(result_search.air_info[0]['coGrade'])])
    coGrade.place(x=420, y=70)
    o3Grade = Label(frame_search,   text=grade_dict[int(result_search.air_info[0]['o3Grade'])])
    o3Grade.place(x=420, y=90)
    no2Grade = Label(frame_search,  text=grade_dict[int(result_search.air_info[0]['no2Grade'])])
    no2Grade.place(x=420, y=110)
    pm10Grade = Label(frame_search, text=grade_dict[int(result_search.air_info[0]['pm10Grade'])])
    pm10Grade.place(x=420, y=130)
    pm25Grade = Label(frame_search, text=grade_dict[int(result_search.air_info[0]['pm25Grade'])])
    pm25Grade.place(x=420, y=170)

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
SearchButton = Button(window, text="검색", command=Search)
SearchButton.place(x=365, y=16)
locationName = Label(frame_search, text='[지역명]                        :  ')
locationName.place(x=10, y=10)
dataTime = Label(frame_search,     text='[일시]                           :  ')
dataTime.place(x=10, y=30)
so2Value = Label(frame_search,     text='[아황산 농도]                 :  ')
so2Value.place(x=10, y=50)
coValue = Label(frame_search,      text='[일산화탄소 농도]           :  ')
coValue.place(x=10, y=70)
o3Value = Label(frame_search,      text='[오존 농도]                    :  ')
o3Value.place(x=10, y=90)
no2Value = Label(frame_search,     text='[이산화질소 농도]           :  ')
no2Value.place(x=10, y=110)
pm10Value = Label(frame_search,    text='[미세먼지 농도]              :  ')
pm10Value.place(x=10, y=130)
pm10Value24 = Label(frame_search,  text='[미세먼지 일 평균 농도]   :  ')
pm10Value24.place(x=10, y=150)
pm25Value = Label(frame_search,    text='[초미세먼지 농도]           :  ')
pm25Value.place(x=10, y=170)
pm25Value24 = Label(frame_search,  text='[초미세먼지 일 평균 농도]:  ')
pm25Value24.place(x=10, y=190)
so2Grade = Label(frame_search,  text='[아황산 지수]      :  ')
so2Grade.place(x=300, y=50)
coGrade = Label(frame_search,   text='[일산화탄소 지수]:  ')
coGrade.place(x=300, y=70)
o3Grade = Label(frame_search,   text='[오존 지수]         :  ')
o3Grade.place(x=300, y=90)
no2Grade = Label(frame_search,  text='[이산화질소 지수]:  ')
no2Grade.place(x=300, y=110)
pm10Grade = Label(frame_search, text='[미세먼지 지수]   :  ')
pm10Grade.place(x=300, y=130)
pm25Grade = Label(frame_search, text='[초미세먼지 지수]:  ')
pm25Grade.place(x=300, y=170)

mapLabel = Label(frame_map, image=info_map.map_png)
mapLabel.pack()
window.mainloop()


#main()


