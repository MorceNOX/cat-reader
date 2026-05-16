function Image(el)
    -- This takes the image source (the URL/path) 
    -- and wraps it in a text string.
    return pandoc.Str("[Image: " .. el.src .. "]")
end
