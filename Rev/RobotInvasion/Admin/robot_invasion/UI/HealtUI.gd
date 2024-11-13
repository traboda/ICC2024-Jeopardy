extends Control

var hearts = 10 setget set_hearts 
var max_hearts = 10 setget set_max_hearts

onready var heartUIFull = $HeartUIFull
onready var heartUIEmpty = $HeartUIEmpty


func set_hearts(value):
	hearts = clamp(value, 0 , max_hearts)
	if heartUIFull != null:
		heartUIFull.rect_size.x = hearts * 15


func set_max_hearts(value):
	max_hearts = max(value, 1)
	self.hearts = min(hearts, max_hearts)
	if heartUIEmpty != null:
		heartUIEmpty.rect_size.x = hearts * 15
	
func _ready():
	self.max_hearts = PlayerStats.max_health
	self.hearts = PlayerStats.health
	var _heart_err  = PlayerStats.connect("health_changed", self, "set_hearts")
	var _health_err = PlayerStats.connect("max_health_changed", self, "set_max_hearts")
