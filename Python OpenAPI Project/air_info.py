class AirInfo:
    id_location = ''
    air_info = []

    def __init__(self, id, info):
        self.id_location = id
        self.air_info = info

    def lastest_info(self):
        if len(self.air_info) is 0:
            return None
        print(self.id_location)
        for info in self.air_info:
            print(info)
        return self.air_info[0]

