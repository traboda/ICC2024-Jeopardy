extends Node2D



export var bullet_speed = 1000
export var fire_rate = 0.2


onready var Bullet = preload("res://Player/Bullet.tscn")
onready var bulletHitBox = $BulletHitBox/CollisionShape2D
var can_fire = true

func _ready():
	pass # Replace with function body.

func _process(_delta):
	look_at(get_global_mouse_position())
	
	if Input.is_action_pressed("fire") and can_fire:
		var bullet = Bullet.instance()
		get_tree().get_root().add_child(bullet)
		bullet.global_position = $BulletPoint.global_position
		bullet.rotation_degrees = rotation_degrees
		bullet.apply_impulse(Vector2(), Vector2(bullet_speed, 0).rotated(rotation))
		can_fire = false
		bulletHitBox.disabled = false
		yield(get_tree().create_timer(fire_rate), "timeout")
		bulletHitBox.disabled = true
		can_fire = true
