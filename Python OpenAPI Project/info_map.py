from xml.dom.minidom import *
from xml.dom.minidom import parse, parseString
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPM
from tkinter import *

class InfoMap:
    map_info = None
    map_png = None
    location_list = []

    def __init__(self):
        self.LoadMapFile()
        print(self.location_list)
        print(len(self.location_list))

    def LoadMapFile(self):
        map_file = open('Administrative_divisions_map_of_South_Korea.svg', 'r', encoding='utf-8')
        map_file_str = map_file.read()
        map_file.close()

        self.map_info = parseString(map_file_str)
        area_list = self.map_info.childNodes[2].childNodes
        print(area_list.length)

        for area in area_list:
            if area.nodeName == 'g' or area.nodeName == 'path':
                print(area.attributes._attrs['id']._value)
                self.location_list.append(area.attributes._attrs['id']._value)
                if area.attributes._attrs['id']._value in '세종특별자치시':
                    print(area.attributes._attrs['fill']._value + ' ' + area.attributes._attrs['stroke']._value)
                    area.attributes._attrs['fill']._value = '#000000'
                subitems = area.childNodes
                for atom in subitems:
                    if atom.attributes is not None:
                        try:
                            print(atom.attributes._attrs['id']._value + ' ' + atom.attributes._attrs['fill']._value + ' ' + atom.attributes._attrs['stroke']._value)
                            self.location_list.append(atom.attributes._attrs['id']._value)
                        except:
                            try:
                                print(atom.attributes._attrs['id']._value)
                                self.location_list.append(atom.attributes._attrs['id']._value)
                            except:
                                print(atom.attributes._attrs['fill']._value + ' ' + atom.attributes._attrs['stroke']._value)

                            if atom.nodeName == 'g' or atom.nodeName == 'path':
                                subatoms = atom.childNodes
                                for part in subatoms:
                                    if part.attributes is not None:
                                        print(part.attributes._attrs['fill']._value + ' ' + part.attributes._attrs['stroke']._value)

    def LoadImage(self):
        self.map_png = PhotoImage(file="file.png")

    def SaveMapFile(self):
        map_file = open('Administrative_divisions_map_of_South_Korea.svg', 'w', encoding='utf-8')
        self.map_info.writexml(map_file)
        map_file.close()

    def MapFileToImage(self):
        drawing = svg2rlg('Administrative_divisions_map_of_South_Korea.svg')
        renderPM.drawToFile(drawing, "file.png")



