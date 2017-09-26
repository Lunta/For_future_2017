from tkinter import *
from tkinter import ttk
import DLSort
from Atmosphere.info_map import *
from Atmosphere.search_location import *

import smtplib
from email.mime.base import MIMEBase
from email.mime.text import MIMEText


# 등급	    좋음	보통	나쁨	매우나쁨
# Grade 값	1	    2	    3	    4
# 항목	    SO2	    CO	        O3	    NO2	         PM10	PM2.5
# 이름      아황산  일산화탄소  오존    이산화질소   미세    초미세
# 단위	    ppm	    ppm	        ppm	    ppm	         ㎍/㎥	㎍/㎥

# smtp 정보
host = "smtp.gmail.com"
port = "587"

window = Tk()
window.geometry("600x800+750+200")
window.title('대기오염정보 조회서비스')

info_map = InfoMap()
result_search = None
grade_dict = dict()
color_dict = dict()
grade_kind = dict()
graph_title = None
canvas_height = 300
email_str = ''

tab = None
frame_search = None
frame_sort = None
frame_map = None
GraphCanvas = None
graph_bar = []
graph_label = []
sort_list = []

SearchEntry = None

mapLabel = None
mapSortList = None
InfoSortList = None
InfoNameList = None
InfoTitleListBox = None
InfoListBox = None

emailEntry = None
emailButton = None

grade_dict[1] = '좋음'
grade_dict[2] = '보통'
grade_dict[3] = '나쁨'
grade_dict[4] = '매우나쁨'
color_dict[1] = 'green'
color_dict[2] = 'yellow'
color_dict[3] = 'red'
color_dict[4] = 'black'
grade_kind['아황산'] = 'so2Grade'
grade_kind['일산화탄소'] = 'coGrade'
grade_kind['오존'] = 'o3Grade'
grade_kind['이산화질소'] = 'no2Grade'
grade_kind['미세먼지'] = 'pm10Grade'
grade_kind['초미세먼지'] = 'pm25Grade'

def Search():
    global result_search, grade_dict, InfoNameList, color_dict, emailEntry, emailButton
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
    if emailEntry is None:
        emailEntry = Entry(frame_search)
        emailEntry.place(x= 250, y=603)
    if emailButton is None:
        emailButton = Button(frame_search, bg='yellow', text="Send E-mail", command=sendmail)
        emailButton.place(x=175, y=600)


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

def SortMapData(event):
    global info_map, InfoSortList, grade_kind, InfoTitleListBox, sort_list
    sort_list.clear()
    info_dict = info_map.LoadInfoDictFromFile()
    for sido in info_dict:
        for dic in info_dict[sido]:
            dic['tag'] = sido
        sort_list += info_dict[sido]
    sort_list.insert(0, grade_kind[mapSortList.get(InfoSortList.curselection()[0])])
    result = list(DLSort.sort(sort_list))
    #print(len(result))
    idx = 0
    if InfoTitleListBox is None:
        InfoTitleListBox = Listbox(frame_sort, width=15, height=45)
        InfoTitleListBox.bind('<<ListboxSelect>>', SelectSortData)
        InfoTitleListBox.pack()
        InfoTitleListBox.place(x=0, y=0)
    else:
        InfoTitleListBox.delete(first=0, last=323)
    for dic in result:
        InfoTitleListBox.insert(idx, str(idx) + '. ' + dic['tag'] + ' ' + dic['stationName'])
        idx += 1

def SelectSortData(event):
    global InfoListBox, sort_list, InfoTitleListBox
    NAME_OF_TAGS = ['[지역명]                        : ', '[일시]                           : ',
                    '[아황산 농도]                 : ', '[일산화탄소 농도]           : ',
                    '[오존 농도]                    : ', '[이산화질소 농도]           : ',
                    '[미세먼지 농도]              : ', '[미세먼지 일 평균 농도]   : ',
                    '[초미세먼지 농도]           : ', '[초미세먼지 일 평균 농도]: ',
                    '[아황산 지수]                 : ', '[일산화탄소 지수]           : ',
                    '[오존 지수]                    : ', '[이산화질소 지수]           : ',
                    '[미세먼지 지수]              : ', '[초미세먼지 지수]           : ']
    if InfoListBox is None:
        InfoListBox = Listbox(frame_sort, width=50, height=45)
        InfoListBox.pack()
        InfoListBox.place(x=120, y=0)
    else:
        InfoListBox.delete(first=0, last=len(NAME_OF_TAGS))
    idx = 0
    for tag in InfoMap.INFO_TAGS:
        InfoListBox.insert(idx, NAME_OF_TAGS[idx] + ' ' + sort_list[InfoTitleListBox.curselection()[0]][tag])
        idx += 1


def SettingLayout():
    global tab, frame_search, frame_sort, frame_map, GraphCanvas, graph_bar, graph_label, mapLabel, mapSortList, \
        InfoNameList, SearchEntry, InfoSortList
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
    dataTime = Label(frame_search, text='[일시]                           :  ')
    dataTime.place(x=10, y=30 + canvas_height)
    so2Value = Label(frame_search, text='[아황산 농도]                 :  ')
    so2Value.place(x=10, y=50 + canvas_height)
    coValue = Label(frame_search, text='[일산화탄소 농도]           :  ')
    coValue.place(x=10, y=70 + canvas_height)
    o3Value = Label(frame_search, text='[오존 농도]                    :  ')
    o3Value.place(x=10, y=90 + canvas_height)
    no2Value = Label(frame_search, text='[이산화질소 농도]           :  ')
    no2Value.place(x=10, y=110 + canvas_height)
    pm10Value = Label(frame_search, text='[미세먼지 농도]              :  ')
    pm10Value.place(x=10, y=130 + canvas_height)
    pm10Value24 = Label(frame_search, text='[미세먼지 일 평균 농도]   :  ')
    pm10Value24.place(x=10, y=150 + canvas_height)
    pm25Value = Label(frame_search, text='[초미세먼지 농도]           :  ')
    pm25Value.place(x=10, y=170 + canvas_height)
    pm25Value24 = Label(frame_search, text='[초미세먼지 일 평균 농도]:  ')
    pm25Value24.place(x=10, y=190 + canvas_height)
    so2Grade = Label(frame_search, text='[아황산 지수]      :  ')
    so2Grade.place(x=300, y=50 + canvas_height)
    coGrade = Label(frame_search, text='[일산화탄소 지수]:  ')
    coGrade.place(x=300, y=70 + canvas_height)
    o3Grade = Label(frame_search, text='[오존 지수]         :  ')
    o3Grade.place(x=300, y=90 + canvas_height)
    no2Grade = Label(frame_search, text='[이산화질소 지수]:  ')
    no2Grade.place(x=300, y=110 + canvas_height)
    pm10Grade = Label(frame_search, text='[미세먼지 지수]   :  ')
    pm10Grade.place(x=300, y=130 + canvas_height)
    pm25Grade = Label(frame_search, text='[초미세먼지 지수]:  ')
    pm25Grade.place(x=300, y=170 + canvas_height)
    GraphCanvas = Canvas(frame_search, bg="white", height=canvas_height - 30, width=550)
    GraphCanvas.place(x=000, y=300)
    GraphCanvas.pack()

    for i in range(8):
        tmp = Label(frame_search, text='')
        tmp.pack()
        tmp.place(x=0, y=0)
        graph_label.append(tmp)

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
    mapSortList.place(x=460, y=0)
    GoodInfoLabel = Label(frame_map, bg='green', text='좋음      ')
    GoodInfoLabel.pack()
    GoodInfoLabel.place(x=20, y=0)
    NormalInfoLabel = Label(frame_map, bg='yellow', text='보통      ')
    NormalInfoLabel.pack()
    NormalInfoLabel.place(x=20, y=20)
    BadInfoLabel = Label(frame_map, bg='red', text='나쁨      ')
    BadInfoLabel.pack()
    BadInfoLabel.place(x=20, y=40)
    TerribleInfoLabel = Label(frame_map, bg='black', text='매우나쁨')
    TerribleInfoLabel.pack()
    TerribleInfoLabel.place(x=20, y=60)

    InfoSortList = Listbox(frame_sort, width=10, height=6)
    InfoSortList.insert(1, '아황산')
    InfoSortList.insert(2, '일산화탄소')
    InfoSortList.insert(3, '오존')
    InfoSortList.insert(4, '이산화질소')
    InfoSortList.insert(5, '미세먼지')
    InfoSortList.insert(6, '초미세먼지')
    InfoSortList.bind('<<ListboxSelect>>', SortMapData)
    InfoSortList.pack()
    InfoSortList.place(x=480, y=0)

    InfoNameList = Listbox(frame_search, width=10, height=6)
    InfoNameList.insert(1, '아황산')
    InfoNameList.insert(2, '일산화탄소')
    InfoNameList.insert(3, '오존')
    InfoNameList.insert(4, '이산화질소')
    InfoNameList.insert(5, '미세먼지')
    InfoNameList.insert(6, '초미세먼지')
    InfoNameList.bind('<<ListboxSelect>>', UpdateGraph)
    InfoNameList.pack()
    InfoNameList.place(x=482, y=0)

def sendmail():
    global host, port, emailEntry, result_search, grade_dict
    html = ""
    title = "Atmosphere Info"
    senderAddr = "tkdqlsek2@gmail.com"
    recipientAddr = emailEntry.get()
    passwd = "!antjs4dlsxjspt!"

    msgtext = ""

    msgtext += '[지역명]                        : ' + result_search.id_location + '\n'
    msgtext += '[일시]                           : ' + result_search.air_info[0]['dataTime'] + '\n'
    msgtext += '[아황산 농도]                 : ' + str(result_search.air_info[0]['so2Value']) + 'ppm' + '\n'
    msgtext += '[일산화탄소 농도]           : ' + str(result_search.air_info[0]['coValue']) + 'ppm' + '\n'
    msgtext += '[오존 농도]                    : ' + str(result_search.air_info[0]['o3Value']) + 'ppm' + '\n'
    msgtext += '[이산화질소 농도]           : ' + str(result_search.air_info[0]['no2Value']) + 'ppm' + '\n'
    msgtext += '[미세먼지 농도]              : ' + str(result_search.air_info[0]['pm10Value']) + '㎍/㎥' + '\n'
    msgtext += '[미세먼지 일 평균 농도]   : ' + str(result_search.air_info[0]['pm10Value24']) + '㎍/㎥' + '\n'
    msgtext += '[초미세먼지 농도]           : ' + str(result_search.air_info[0]['pm25Value']) + '㎍/㎥' + '\n'
    msgtext += '[초미세먼지 일 평균 농도]: ' + str(result_search.air_info[0]['pm25Value24']) + '㎍/㎥' + '\n'
    msgtext += '[아황산 지수]                 : ' + grade_dict[int(result_search.air_info[0]['so2Grade'])] + '\n'
    msgtext += '[일산화탄소 지수]           : ' + grade_dict[int(result_search.air_info[0]['coGrade'])] + '\n'
    msgtext += '[오존 지수]                    : ' + grade_dict[int(result_search.air_info[0]['o3Grade'])] + '\n'
    msgtext += '[이산화질소 지수]           : ' + grade_dict[int(result_search.air_info[0]['no2Grade'])] + '\n'
    msgtext += '[미세먼지 지수]              : ' + grade_dict[int(result_search.air_info[0]['pm10Grade'])] + '\n'
    msgtext += '[초미세먼지 지수]           : ' + grade_dict[int(result_search.air_info[0]['pm25Grade'])] + '\n'

    msg = MIMEBase("multipart", "alternative")
    msg['Subject'] = title
    msg['From'] = senderAddr
    msg['To'] = recipientAddr

    # MIME 문서를 생성합니다.
    HtmlPart = MIMEText(msgtext, 'plain', _charset='UTF-8')

    # 만들었던 mime을 MIMEBase에 첨부 시킨다.
    msg.attach(HtmlPart)

    print("connect smtp server ... ")
    s = smtplib.SMTP(host, port)
    # s.set_debuglevel(1)
    s.ehlo()
    s.starttls()
    s.ehlo()
    s.login(senderAddr, passwd)  # 로그인을 합니다.
    s.sendmail(senderAddr, [recipientAddr], msg.as_string())
    s.close()

    print("Mail sending complete!!!")

def Run():
    global window
    SettingLayout()
    window.mainloop()

if __name__ == '__main__':
    Run()


