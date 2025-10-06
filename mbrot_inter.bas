#!/usr/bin/env prismshell
' Interactive Mandelbrot Set Explorer

' Check color support
CALL Color.Supported()
LET HAS_COLOR = _

' Display welcome and rendering options
CALL Color.Bold()
CALL Color.Cyan()
PRINT _; "========================================="
PRINT _; "  Interactive Mandelbrot Set Explorer"
PRINT _; "========================================="
CALL Color.Reset()
PRINT _
PRINT ""

IF HAS_COLOR == 1 THEN
  PRINT "Color support detected!"
  PRINT ""
  PRINT "Choose rendering style:"
  PRINT "  1. Color + Density glyphs (@ # % & $ + * .)"
  PRINT "  2. Color blocks (solid █ characters)"
  PRINT "  3. Monochrome ASCII (grayscale glyphs)"
  PRINT ""
  INPUT RENDER_MODE
  
  ' Validate input
  IF RENDER_MODE < 1 OR RENDER_MODE > 3 THEN
    PRINT "Invalid choice, using mode 1"
    LET RENDER_MODE = 1
  ENDIF
ELSE
  PRINT "No color support detected."
  PRINT "Using monochrome ASCII rendering."
  LET RENDER_MODE = 3
ENDIF

' Initialize view parameters
LET center_x = -0.5
LET center_y = 0
LET zoom = 1.5
LET max_iter = 100
LET running = 1
LET need_render = 1

' Main loop
WHILE running == 1
  ' Only render if needed
  IF need_render == 1 THEN
    ' Calculate bounds
    LET width = 3.0 / zoom
    LET height = 2.0 / zoom
    LET min_x = center_x - width / 2
    LET max_x = center_x + width / 2
    LET min_y = center_y - height / 2
    LET max_y = center_y + height / 2
    
    ' Generate mandelbrot data
    PRINT ""
    PRINT "Generating..."
    DIM mandel[40, 80]
    
    FOR py = 0 TO 39
      FOR px = 0 TO 79
        ' Map pixel to complex plane
        LET x0 = min_x + (px / 80) * (max_x - min_x)
        LET y0 = min_y + (py / 40) * (max_y - min_y)
        
        LET x = 0
        LET y = 0
        LET iter = 0
        
        ' Mandelbrot iteration
        WHILE iter < max_iter AND (x*x + y*y) < 4
          LET xtemp = x*x - y*y + x0
          LET ytemp = 2*x*y + y0
          LET x = xtemp
          LET y = ytemp
          LET iter = iter + 1
        WEND
        
        LET mandel[py, px] = iter
      NEXT px
    NEXT py
    
    PRINT "Rendering..."
    PRINT ""
    
    ' Render based on mode
    FOR py = 0 TO 39
      FOR px = 0 TO 79
        LET val = mandel[py, px]
        
        IF RENDER_MODE == 1 THEN
          ' Color + glyphs
          CALL RenderColorGlyph(val, max_iter)
        ELSEIF RENDER_MODE == 2 THEN
          ' Color blocks
          CALL RenderColorBlock(val, max_iter)
        ELSE
          ' Monochrome
          CALL RenderMono(val, max_iter)
        ENDIF
      NEXT px
      
      IF RENDER_MODE == 1 OR RENDER_MODE == 2 THEN
        CALL Color.Reset()
        PRINT _
      ELSE
        PRINT ""
      ENDIF
    NEXT py
    
    ' Display info
    CALL Color.Reset()
    PRINT _
    PRINT ""
    CALL Color.Bold()
    PRINT _; "View Info:"
    CALL Color.Reset()
    PRINT _
    PRINT "  Center: ("; center_x; ", "; center_y; ")"
    PRINT "  Zoom: "; zoom; "x"
    PRINT "  Max Iterations: "; max_iter
    PRINT ""
    
    ' Reset render flag after rendering
    LET need_render = 0
  ENDIF
  
  ' Always show menu (outside the render check)
  CALL Color.Bold()
  CALL Color.Yellow()
  PRINT _; "Navigation Menu:"
  CALL Color.Reset()
  PRINT _
  PRINT "  w/s - Move up/down"
  PRINT "  a/d - Move left/right"
  PRINT "  +/- - Zoom in/out"
  PRINT "  i   - Increase max iterations"
  PRINT "  r   - Reset view"
  PRINT "  m   - Change render mode"
  PRINT "  q   - Quit"
  PRINT ""
  
  INPUT choice

  ' Trim any whitespace and convert to uppercase for comparison
  LET choice_upper = choice
  
  ' Process choice
  IF choice == "w" OR choice == "W" THEN
    LET center_y = center_y - 0.1 / zoom
    LET need_render = 1
  ELSEIF choice == "s" OR choice == "S" THEN
    LET center_y = center_y + 0.1 / zoom
    LET need_render = 1
  ELSEIF choice == "a" OR choice == "A" THEN
    LET center_x = center_x - 0.1 / zoom
    LET need_render = 1
  ELSEIF choice == "d" OR choice == "D" THEN
    LET center_x = center_x + 0.1 / zoom
    LET need_render = 1
  ELSEIF choice == "+" OR choice == "=" THEN
    LET zoom = zoom * 2
    PRINT "Zoomed in to "; zoom; "x"
    LET need_render = 1
  ELSEIF choice == "-" OR choice == "_" THEN
    LET zoom = zoom / 2
    IF zoom < 0.5 THEN
      LET zoom = 0.5
      PRINT "Minimum zoom reached"
    ELSE
      PRINT "Zoomed out to "; zoom; "x"
    ENDIF
    LET need_render = 1
  ELSEIF choice == "i" OR choice == "I" THEN
    LET max_iter = max_iter + 50
    PRINT "Max iterations increased to: "; max_iter
    PRINT ""
    LET need_render = 1
  ELSEIF choice == "r" OR choice == "R" THEN
    LET center_x = -0.5
    LET center_y = 0
    LET zoom = 1.5
    LET max_iter = 100
    PRINT "View reset to default"
    PRINT ""
    LET need_render = 1
  ELSEIF choice == "m" OR choice == "M" THEN
    IF HAS_COLOR == 0 THEN
      PRINT "Color modes not available"
      PRINT ""
    ELSE
      PRINT ""
      PRINT "Choose rendering style:"
      PRINT "  1. Color + Density glyphs"
      PRINT "  2. Color blocks"
      PRINT "  3. Monochrome ASCII"
      PRINT ""
      INPUT RENDER_MODE
      IF RENDER_MODE < 1 OR RENDER_MODE > 3 THEN
        LET RENDER_MODE = 1
        PRINT "Invalid, using mode 1"
      ENDIF
      PRINT ""
      LET need_render = 1
    ENDIF
  ELSEIF choice == "q" OR choice == "Q" THEN
    LET running = 0
  ELSEIF choice == "" THEN
    PRINT "No input, showing menu again"
    PRINT ""
  ELSE
    PRINT "Invalid choice: '"; choice; "'"
    PRINT ""
  ENDIF
WEND

' Exit
CALL Color.Reset()
PRINT _
PRINT "Thanks for exploring!"
END

' ===== SUBROUTINES =====

SUB RenderColorGlyph(val, max_val)
  IF val >= max_val THEN
    CALL Color.Black()
    LET _ = _
    PRINT _; "@";
  ELSEIF val >= max_val * 0.8 THEN
    CALL Color.Blue()
    LET _ = _
    PRINT _; "#";
  ELSEIF val >= max_val * 0.6 THEN
    CALL Color.Blue()
    CALL Color.Bold()
    LET _ = _
    PRINT _; "%";
  ELSEIF val >= max_val * 0.4 THEN
    CALL Color.Cyan()
    LET _ = _
    PRINT _; "&";
  ELSEIF val >= max_val * 0.3 THEN
    CALL Color.Green()
    LET _ = _
    PRINT _; "$";
  ELSEIF val >= max_val * 0.2 THEN
    CALL Color.Yellow()
    LET _ = _
    PRINT _; "+";
  ELSEIF val >= max_val * 0.1 THEN
    CALL Color.Red()
    CALL Color.Bold()
    LET _ = _
    PRINT _; "*";
  ELSEIF val >= max_val * 0.05 THEN
    CALL Color.Red()
    LET _ = _
    PRINT _; ".";
  ELSE
    CALL Color.Magenta()
    LET _ = _
    PRINT _; " ";
  ENDIF
END SUB

SUB RenderColorBlock(val, max_val)
  IF val >= max_val THEN
    CALL Color.Black()
    LET _ = _
    PRINT _; "█";
  ELSEIF val >= max_val * 0.8 THEN
    CALL Color.Blue()
    LET _ = _
    PRINT _; "█";
  ELSEIF val >= max_val * 0.6 THEN
    CALL Color.Blue()
    CALL Color.Bold()
    LET _ = _
    PRINT _; "█";
  ELSEIF val >= max_val * 0.4 THEN
    CALL Color.Cyan()
    LET _ = _
    PRINT _; "█";
  ELSEIF val >= max_val * 0.3 THEN
    CALL Color.Green()
    LET _ = _
    PRINT _; "█";
  ELSEIF val >= max_val * 0.2 THEN
    CALL Color.Yellow()
    LET _ = _
    PRINT _; "█";
  ELSEIF val >= max_val * 0.1 THEN
    CALL Color.Red()
    CALL Color.Bold()
    LET _ = _
    PRINT _; "█";
  ELSEIF val >= max_val * 0.05 THEN
    CALL Color.Red()
    LET _ = _
    PRINT _; "█";
  ELSE
    CALL Color.Magenta()
    LET _ = _
    PRINT _; "█";
  ENDIF
END SUB

SUB RenderMono(val, max_val)
  IF val >= max_val THEN
    PRINT "@";
  ELSEIF val >= max_val * 0.8 THEN
    PRINT "#";
  ELSEIF val >= max_val * 0.6 THEN
    PRINT "%";
  ELSEIF val >= max_val * 0.4 THEN
    PRINT "&";
  ELSEIF val >= max_val * 0.3 THEN
    PRINT "$";
  ELSEIF val >= max_val * 0.2 THEN
    PRINT "+";
  ELSEIF val >= max_val * 0.1 THEN
    PRINT "*";
  ELSEIF val >= max_val * 0.05 THEN
    PRINT ".";
  ELSE
    PRINT " ";
  ENDIF
END SUB