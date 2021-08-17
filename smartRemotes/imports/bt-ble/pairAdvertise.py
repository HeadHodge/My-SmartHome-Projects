#!/usr/bin/python
# SPDX-License-Identifier: LGPL-2.1-or-later

import os, time

def start(options={"btmgmt": {}}):
    print('start advertising')

    #start advertising
    #print(' \n***ADVERTISE: Host device not connected');
    #os.system("hcitool -i hci0 cmd 0x08 0x0006 50 00 50 00 00 00 00 00 00 00 00 00 00 07 00")
    #os.system("hcitool -i hci0 cmd 0x08 0x000a 01")

    for key, value in options["btmgmt"].items():
        command = f'btmgmt {key} {value}'
        print(os.system(command), f' : {key} {value}')   
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--timeout', default=0, type=int, help="advertise " +
                        "for this many seconds then stop, 0=run forever " +
                        "(default: 0)")
    args = parser.parse_args()

    main(args.timeout)
