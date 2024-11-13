extends RigidBody2D

onready var bulletHitBox = $BulletHitBox


func _process(_delta):
	bulletHitBox.knockback_vector = linear_velocity.normalized()

func _on_Bullet_body_entered(_body):
	queue_free()


func _on_notifier_screen_exited():
	queue_free()
