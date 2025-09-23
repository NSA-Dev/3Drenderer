# debug_commands.gdb

# Set breakpoints
break main                    # stop at the start of main
break setup                    # stop at setup() to inspect model/texture loading
break process_input            # stop at first input processing
break update                   # stop at update() to inspect camera/mesh matrices
break draw                     # stop at draw() to inspect rendered triangles

# Run the program
run

# Automatically display variables when stopped
display defaultRenderer.isRunning
display defaultRenderer.cameraPtr->position
display defaultRenderer.cameraPtr->forwardVelocity
display defaultRenderer.cameraPtr->yaw
display defaultRenderer.meshPtr->translation
display defaultRenderer.meshPtr->rotation
display defaultRenderer.renderingMode
display defaultRenderer.cullMethod
display defaultRenderer.lightMethod
display defaultRenderer.triangleCount

# Optional: continue execution to next breakpoint
# continue

# Example of printing a single triangle after entering draw()
# print defaultRenderer.renderQueue[0]
