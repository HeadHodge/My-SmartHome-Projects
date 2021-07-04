####################################
##       NOTE TOOL
####################################
print('Load note')
import sys, time, json, traceback, asyncio

#############################################
def object2serial(note):
#############################################
    #print(f'object2serial')
    return json.dumps(note)
 
#############################################
def serial2object(note):
#############################################
    #print(f'serial2object')
    return json.loads(note)
   
#############################################
def publishNote(author, title, content):
#############################################
    #print(f'publishNote')

    note = {
        'author' : author,
        'title'  : title,
        'host'   : 'hubNode',
        'time'   : int(time.time() * 1000),
        'content': content
    }
    
    return note
    
#######################################
#              MAIN
#######################################

if __name__ == '__main__':
    noop
