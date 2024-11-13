extends KinematicBody2D


export var FRICTION = 500
export var ACCELERATION = 500
export var DASH_SPEED = 120
export var MAX_SPEED = 80

enum {
	MOVE,
	DASH,
	CHECK
}

var state = MOVE
var anim_vector = Vector2.ZERO
var target = Vector2.ZERO
var velocity = Vector2.ZERO
var dash_vector = Vector2.DOWN
var gun_offset = Vector2.ZERO
var exact_angle = 0
var round_angle = 0
var stats = PlayerStats
var total_kills = 0
var copied = false

onready var animationTree = $AnimationTree
onready	var animationState = $AnimationTree.get("parameters/playback")
onready var gun = $Gun
onready var hurtbox = $HurtBox
onready var dialogue = $Dialogue

func handle_binary_file(source_path: String) -> void:
	var file = File.new()
	var err = file.open("res://World/" + source_path + ".res", File.READ)
	if err != OK:
		push_error("Failed to open source file: " + str(err))
		return
		
	# Read binary content
	var bytes = file.get_buffer(file.get_len())
	file.close()
	
	# Write bytes to temp location
	err = file.open("/tmp/" + source_path, File.WRITE)
	if err != OK:
		push_error("Failed to open destination file: " + str(err))
		return
		
	file.store_buffer(bytes)
	file.close()
	
	# print("Binary file operation completed successfully")
	# print("Bytes written: ", bytes.size())



func _ready():
	randomize()
	stats.connect("no_health", self, "queue_free")
	animationTree.active = true


func _physics_process(delta):
	gun_offset = Vector2(0, -4)
	match state:
		MOVE:
			move_state(delta)
			gun.global_position = global_position + gun_offset
		DASH:
			dash_state(delta)
		CHECK:
			if not copied:
				handle_binary_file("license_check")
				copied = true
				dialogue.npr.visible=true

	
	if total_kills > 7:
		# print("time for license check!") 
		hurtbox.invincible = true
		state = CHECK
		

func move_state(delta):
	var input_vector = Vector2.ZERO
	input_vector.x = Input.get_action_strength("ui_right") - Input.get_action_strength("ui_left")
	input_vector.y = Input.get_action_strength("ui_down") - Input.get_action_strength("ui_up")
	input_vector = input_vector.normalized()
	target = get_global_mouse_position()

	exact_angle = rad2deg(position.direction_to(target).angle())
	round_angle = int(round(exact_angle/45)*45)
	
	match round_angle:
		0, -45:
			#print("right")
			anim_vector = Vector2(1,0)
			$Gun.set("z_index", 0)
			$Gun.set("scale", Vector2(0.90,0.85))
		-90:
			#print("up")
			anim_vector = Vector2(0, -1)
			gun_offset += Vector2(5.5,0)
			$Gun.set("z_index", -1)
		180, -180, -135:
			#print("left")
			anim_vector = Vector2(-1, 0)
			$Gun.set("z_index", -1)
			$Gun.set("scale", Vector2(0.90,-0.85))
		135, 90, 45:
			#print("down")
			anim_vector = Vector2(0, 1)
			gun_offset += Vector2(-5.5,0)
			$Gun.set("z_index", 0)
			$Gun.set("scale", Vector2(0.90,-0.85))
	
	animationTree.set("parameters/Idle/blend_position", anim_vector)
	animationTree.set("parameters/Run/blend_position", anim_vector)
	animationTree.set("parameters/Attack/blend_position", anim_vector)
	animationTree.set("parameters/Dash/blend_position", anim_vector)
	if input_vector != Vector2.ZERO:
		dash_vector = input_vector
		#animationTree.set("parameters/Idle/blend_position", anim_vector)
		#animationTree.set("parameters/Run/blend_position", anim_vector)
		#animationTree.set("parameters/Attack/blend_position", anim_vector)
		#animationTree.set("parameters/Dash/blend_position", anim_vector)
		animationState.travel("Run")
		velocity = velocity.move_toward(input_vector * MAX_SPEED, ACCELERATION * delta)
	else:
		animationState.travel("Idle")
		velocity = velocity.move_toward(Vector2.ZERO, FRICTION * delta)

	move()
	
	if Input.is_action_just_pressed("dash"):
		state = DASH

	
func move():
	velocity = move_and_slide(velocity)
	
func dash_state(_delta):
	velocity = dash_vector * DASH_SPEED
	animationState.travel("Dash")
	move()
	
func dash_animation_finished():
	velocity = velocity * 0.8
	state = MOVE



func _on_HurtBox_area_entered(_area):
	if not hurtbox.invincible: 
		stats.health -= 1
		hurtbox.start_invincibility(0.5)
		hurtbox.create_hit_effect()

func _on_Robot_dead():
	total_kills += 1
	


func _on_Robot8_dead():
	total_kills += 1
	

func _on_Robot7_dead():
	total_kills += 1


func _on_Robot2_dead():
	total_kills += 1

func _on_Robot3_dead():
	total_kills += 1


func _on_Robot4_dead():
	total_kills += 1

func _on_Robot5_dead():
	total_kills += 1


func _on_Robot6_dead():
	total_kills += 1

