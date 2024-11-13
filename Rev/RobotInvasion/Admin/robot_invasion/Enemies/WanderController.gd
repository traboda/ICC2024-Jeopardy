extends Node2D

export(int)  var wrange = 32

onready var start_position = global_position
onready var target_position = global_position

onready var timer = $Timer

func _ready():
	update_target_position()

func update_target_position():
	var target_vector = Vector2(rand_range(-wrange, wrange), rand_range(-wrange, wrange))
	target_position = start_position + target_vector
	
func get_time_left():
	return timer.time_left
	
func set_wander_timer(duration):
	timer.start(duration)

func _on_Timer_timeout():
	update_target_position()
