#############################################
##     gatt livingRoom controlCommands
#############################################
print('Load gatt livingRoom controlCommands')

longPress = .5 #seconds

commands = {
	"Home": [
		{"controlWord": "Home", "hidCode": 0x223, "hidReport": 2}
	],
	
	"Menu": [
		{"controlWord": "Menu", "hidCode": 0x40, "hidReport": 2}
	],
		
	"Back": [
		{"controlWord": "Back", "hidCode": 0x46, "hidReport": 2}
	],
	
	"Up": [
		{"controlWord": "Up", "hidCode": 0x42, "hidReport": 2}
	],
	
	"Down": [
		{"controlWord": "Down", "hidCode": 0x43, "hidReport": 2}
	],
	
	"Left": [
		{"controlWord": "Left", "hidCode": 0x44, "hidReport": 2}
	],
	
	"Right": [
		{"controlWord": "Right", "hidCode": 0x45, "hidReport": 2}
	],
	
	"Ok": [
		{"controlWord": "Ok", "hidCode": 0x41, "hidReport": 2}
	],
	
	"Louder": [
		{"controlWord": "Louder", "hidCode": 0xE9, "hidReport": 2}
	],
	
	"Softer": [
		{"controlWord": "Softer", "hidCode": 0xEA, "hidReport": 2}
	],
	
	"Silence": [
		{"controlWord": "Menu", "hidCode": 0x40, "hidReport": 2}
	],
	
	"Sound": [
		{"controlWord": "Menu", "hidCode": 0x40, "hidReport": 2}
	],
    
	"SoundToggle": [
		{"controlWord": "SoundToggle", "hidCode": 0xE2, "hidReport": 2}
	],
		
	"Forward": [
		{"controlWord": "Forward", "hidCode": 0xB3, "hidReport": 2, "hidWait": longPress, "hidRepeat": 1}
	],
	
	"Backward": [
		{"controlWord": "Backward", "hidCode": 0xB4, "hidReport": 2, "hidWait": longPress, "hidRepeat": 1}
	],

	"PlayToggle": [
		{"controlWord": "PlayToggle", "hidCode": 0xCD, "hidReport": 2}
	],
}
