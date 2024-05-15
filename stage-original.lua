-- A simples test of LIM

--[[

    Try also:-----------------++
                              ||
    TinyLibrary               ||
     |                        \/
     +-------> [https://github.com/duckafire/TinyLibrary]

]]

-- See the "rules" of the LIM



-- Using TIC80 functions

local function statusBar(x, y, current, max, width, Height, backColor, frontColor, isVertical)
    -- left to right
    width  = current * (width / max)

    -- down to top
    if isVertical then
        width  = width
        Height = current * (width / max)
    end

    -- draw
    rect(x, y, width, Height, backColor ) -- background
    rect(x, y, width, Height, frontColor) -- front
end

local function cursorIcon(spt, chroma)
    -- hide mouse cursor
    poke(0x03ffb, 0)

    -- cursor position
    local x, y = mouse()

    -- default chroma color
    chroma = chroma or 0

    -- draw
    spr(spt, x, y, chroma)
end

local function pauseGame(lock)
    -- invert boolean
    if lock then
        return false
    end

    -- default
    return true
end

local function info(_timer, points, x, y, color)
    -- simple game informations
    print('Timer: '.._time.."\n Points: "..points, x, y, color)
end

--[[ END ]]--
