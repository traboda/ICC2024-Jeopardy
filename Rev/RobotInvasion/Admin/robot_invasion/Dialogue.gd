extends CanvasLayer


# Declare member variables here. Examples:
# var a = 2
# var b = "text"

onready var npr = $NinePatchRect

# Called when the node enters the scene tree for the first time.
func _ready():
	$NinePatchRect.visible = false


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_LineEdit_text_entered(new_text):
	if len(new_text) == 35:
		#print("HERE!")
		var array = [new_text]
		var array2 = ["+x", "/tmp/license_check"]
		var 	args = PoolStringArray(array)
		var args2 = PoolStringArray(array2)
		var res1 = OS.execute("chmod", args2, true)
		#print("HERE2", str(res1))
		var res = OS.execute("/tmp/license_check", args, true)
		#print("Result: ", str(res))
		if res == 1:
			$NinePatchRect.rtl.text = "Correct!"
	
