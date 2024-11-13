extends KinematicBody2D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

onready var stats = $Stats
onready var playerDectectionZOne = $PlayerDetectionZone
onready var animationTree = $AnimationTree
onready var animationState = $AnimationTree.get("parameters/playback")
onready var hurtbox = $HurtBox
onready var softCollision = $SoftCollision
onready var wanderController = $WanderController

signal dead

const EnemyDeathEffect = preload("res://Effects/EnemyDeathEffect.tscn")

# Called when the node enters the scene tree for the first time.

var knockback = Vector2.ZERO
export var knockback_value = 100
export var ACCELERATION =  300
export var MAX_SPEED = 50
export var FRICTION = 200 

enum {
	IDLE,
	WANDER,
	CHASE
}

var velocity = Vector2.ZERO
var state = IDLE

func _ready():
	animationTree.active = true
	state = pick_random_state([IDLE, WANDER])
		

func seek_player():
	if playerDectectionZOne.can_see_player():
		state = CHASE

func pick_random_state(state_list):
	state_list.shuffle()
	return state_list.pop_front()

func _physics_process(delta):
	knockback = knockback.move_toward(Vector2.ZERO, FRICTION * delta)
	knockback = move_and_slide(knockback)

	match state:
		IDLE:
			animationState.travel("Idle")
			velocity = velocity.move_toward(Vector2.ZERO, FRICTION * delta)
			seek_player()
			
			if wanderController.get_time_left() == 0:
				update_wander()
				
		WANDER:
			seek_player()
			
			if wanderController.get_time_left() == 0:
				update_wander()
			
			accelerate_towards_point(wanderController.target_position, delta)
			
			if global_position.distance_to(wanderController.target_position) <= 4:
				update_wander()
				
		CHASE:
			var player = playerDectectionZOne.player
			if player != null:
				accelerate_towards_point(player.global_position, delta)
			else:
				state = IDLE	
				
	if softCollision.is_colliding():
		velocity += softCollision.get_push_vector() * delta * 400
	velocity = move_and_slide(velocity)

func update_wander():
	state = pick_random_state([IDLE, WANDER])
	wanderController.set_wander_timer(rand_range(1,3))
	
func accelerate_towards_point(point, delta):
	var direction = global_position.direction_to(point)
	velocity = velocity.move_toward(direction * MAX_SPEED, ACCELERATION * delta)
	animationTree.set("parameters/Idle/blend_position", velocity.normalized())
	animationTree.set("parameters/Run/blend_position", velocity.normalized())
	animationState.travel("Run")

func _on_HurtBox_area_entered(area):
	stats.health -= area.damage
	knockback = area.knockback_vector * knockback_value
	hurtbox.start_invincibility(0.2)
	hurtbox.create_hit_effect()

func _on_Stats_no_health():
	emit_signal("dead")
	queue_free()
	var enemyDeathEffect = EnemyDeathEffect.instance()
	get_parent().add_child(enemyDeathEffect)
	enemyDeathEffect.global_position = global_position

