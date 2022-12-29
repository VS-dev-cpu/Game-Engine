object = class()

function object.onCreate()
end

function object.onUpdate(deltaTime, millis)
    -- object.rotation.y = math.sin(millis / 1000)*360
end

function object.onDestroy()
    print("object destroyed")
end
