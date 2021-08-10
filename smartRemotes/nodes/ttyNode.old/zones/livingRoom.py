#############################################
##    Load livingRoom wordMap
#############################################
print('Load livingRoom wordMap')

#########################################################################################################################################################################################################################################################################################
#keyboard report1: byte 1:static 0xff, 2:reportId 0x01, 3: modifier byte: bit 0 is L CTRL, bit 1 is L SHIFT, bit 2 is L ALT, bit 3 is L GUI, bit 4 is R CTRL, bit 5 is R SHIFT, bit 6 is R ALT, and bit 7 is R GUI, 4: low byte of consumer keyCode, 5: reserved 0x00, 6: reserved 0x00
#consumer report2: byte 1:static 0xff, 2:reportId 0x02, 3: low byte of consumer keyCode, 4: high byte of consumer keyCode, 5: reserved 0x00, 6: reserved 0x00
#mouse report3   : byte 1:static 0xff, 2:reportId 0x03, 3: modifier byte: bit 0 is left Click, bit 1 middle Click, bit 2 is right Click, , 4: x-axis offset, 5: y-axis offset, 6: reserved 0x00
#########################################################################################################################################################################################################################################################################################
wordMap = {
	"Home": [
		{"deviceCommand": [0xff, 0x02, 0x23, 0x02, 0x00, 0x00], "controlWord": "Home", "hidCode": 0x223, "options": "reload"}
	],
	
	"Menu": [
		{"deviceCommand": [0xff, 0x02, 0x40, 0x00, 0x00, 0x00], "controlWord": "Menu", "hidCode": 0x40}
	],
		
	"Back": [
		{"deviceCommand": [0xff, 0x02, 0x46, 0x00, 0x00, 0x00], "controlWord": "Back", "hidCode": 0x46}
	],
	
	"Up": [
		{"deviceCommand": [0xff, 0x02, 0x42, 0x00, 0x00, 0x00]}
	],
	
	"Down": [
		{"deviceCommand": [0xff, 0x02, 0x43, 0x00, 0x00, 0x00]}
	],
	
	"Left": [
		{"deviceCommand": [0xff, 0x02, 0x44, 0x00, 0x00, 0x00]}
	],
	
	"Right": [
		{"deviceCommand": [0xff, 0x02, 0x45, 0x00, 0x00, 0x00]}
	],
	
	"Ok": [
		{"deviceCommand": [0xff, 0x02, 0x41, 0x00, 0x00, 0x00]}
	],
	
	"Softer": [
		{"deviceCommand": [0xff, 0x02, 0xEA, 0x00, 0x00, 0x00]}
	],
	
	"Louder": [
		{"deviceCommand": [0xff, 0x02, 0xE9, 0x00, 0x00, 0x00]}
	],
	
	"Silence": [
		{"deviceCommand": [0xff, 0x02, 0xE2, 0x00, 0x00, 0x00]}
	],
	
	"Sound": [
		{"deviceCommand": [0xff, 0x02, 0xE2, 0x00, 0x00, 0x00]}
	],
				
	"SoundToggle": [
		{"deviceCommand": [0xff, 0x02, 0xE2, 0x00, 0x00, 0x00]}
	],
				
	"Forward": [
		{"deviceCommand": [0xff, 0x02, 0xB3, 0x00, 0x00, 0x00], "keyPressSecs": .25}
	],
	
	"Backward": [
		{"deviceCommand": [0xff, 0x02, 0xB4, 0x00, 0x00, 0x00], "keyPressSecs": .25}
	],
	
	"PlayToggle": [
		{"deviceCommand": [0xff, 0x02, 0xCD, 0x00, 0x00, 0x00]}
	],
	
	"On": [
		{"deviceCommand": [0xff, 0x02, 0x23, 0x02, 0x00, 0x00]},
	],
		
	"OnToggle": [
		{"deviceCommand": [0xff, 0x02, 0x30, 0x00, 0x00, 0x00]},
	]
}
