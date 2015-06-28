-----------------------------------------------------------------------------------
-- Maratis
-- Render to texture script test
-----------------------------------------------------------------------------------

-- get objects
TPot = getObject("TPot")
Camera2 = getObject("Camera2")

-- enable camera render to texture
enableRenderToTexture(Camera2, "maps/lenna.jpg", 512, 512)


-- scene update
function onSceneUpdate()

	rotate(TPot, {0, 0, -1}, 1)
	
end

 
func