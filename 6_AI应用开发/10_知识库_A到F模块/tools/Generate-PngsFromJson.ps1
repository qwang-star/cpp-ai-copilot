$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

$Root = Split-Path -Parent $PSScriptRoot
$OutDir = Join-Path $Root "_images"
$ConfigPath = Join-Path $PSScriptRoot "diagram_config.json"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

function New-Color($hex) {
    $h = $hex.TrimStart("#")
    return [System.Drawing.Color]::FromArgb(
        [Convert]::ToInt32($h.Substring(0, 2), 16),
        [Convert]::ToInt32($h.Substring(2, 2), 16),
        [Convert]::ToInt32($h.Substring(4, 2), 16)
    )
}

function Draw-RoundedRect($g, $brush, [float]$x, [float]$y, [float]$w, [float]$h, [float]$r) {
    $path = New-Object System.Drawing.Drawing2D.GraphicsPath
    $d = $r * 2
    $path.AddArc($x, $y, $d, $d, 180, 90)
    $path.AddArc($x + $w - $d, $y, $d, $d, 270, 90)
    $path.AddArc($x + $w - $d, $y + $h - $d, $d, $d, 0, 90)
    $path.AddArc($x, $y + $h - $d, $d, $d, 90, 90)
    $path.CloseFigure()
    $g.FillPath($brush, $path)
    $path.Dispose()
}

function Draw-WrappedText($g, [string]$text, $font, $brush, [float]$x, [float]$y, [float]$maxWidth, [float]$lineHeight) {
    $line = ""
    $lines = New-Object System.Collections.Generic.List[string]
    foreach ($ch in $text.ToCharArray()) {
        $try = $line + $ch
        if ($g.MeasureString($try, $font).Width -gt $maxWidth -and $line.Length -gt 0) {
            $lines.Add($line)
            $line = [string]$ch
        } else {
            $line = $try
        }
    }
    if ($line.Length -gt 0) { $lines.Add($line) }
    for ($i = 0; $i -lt [Math]::Min(2, $lines.Count); $i++) {
        $g.DrawString($lines[$i], $font, $brush, $x, $y + $i * $lineHeight)
    }
}

function New-FlowPng($diagram, [string]$footer) {
    $w = 1600
    $h = 980
    $bmp = New-Object System.Drawing.Bitmap $w, $h
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::ClearTypeGridFit

    $bg = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
        (New-Object System.Drawing.Rectangle 0,0,$w,$h),
        (New-Color "#f8fafc"),
        (New-Color "#ecfeff"),
        [System.Drawing.Drawing2D.LinearGradientMode]::ForwardDiagonal
    )
    $g.FillRectangle($bg, 0, 0, $w, $h)

    $headBrush = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
        (New-Object System.Drawing.Rectangle 55,50,1490,126),
        (New-Color $diagram.accent),
        (New-Color $diagram.accent2),
        [System.Drawing.Drawing2D.LinearGradientMode]::Horizontal
    )
    Draw-RoundedRect $g $headBrush 55 50 1490 126 26

    $titleFont = New-Object System.Drawing.Font("Microsoft YaHei", 30, [System.Drawing.FontStyle]::Bold)
    $subFont = New-Object System.Drawing.Font("Microsoft YaHei", 17, [System.Drawing.FontStyle]::Regular)
    $white = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::White)
    $g.DrawString([string]$diagram.title, $titleFont, $white, 95, 85)
    $g.DrawString([string]$diagram.subtitle, $subFont, $white, 98, 132)

    $cardW = 310
    $cardH = 96
    $gapX = 70
    $gapY = 74
    $startX = 95
    $startY = 205
    $cols = 4
    $accentBrush = New-Object System.Drawing.SolidBrush (New-Color $diagram.accent)
    $textBrush = New-Object System.Drawing.SolidBrush (New-Color "#0f172a")
    $mutedPen = New-Object System.Drawing.Pen (New-Color "#64748b"), 4
    $cardFont = New-Object System.Drawing.Font("Microsoft YaHei", 18, [System.Drawing.FontStyle]::Bold)
    $numFont = New-Object System.Drawing.Font("Segoe UI", 13, [System.Drawing.FontStyle]::Bold)

    for ($i = 0; $i -lt $diagram.steps.Count; $i++) {
        $row = [Math]::Floor($i / $cols)
        $col = $i % $cols
        $x = $startX + $col * ($cardW + $gapX)
        $y = $startY + $row * ($cardH + $gapY)
        if ($i % 2 -eq 0) {
            $cardColor = [System.Drawing.Color]::White
        } else {
            $cardColor = New-Color "#fefce8"
        }
        $cardBrush = New-Object System.Drawing.SolidBrush $cardColor
        Draw-RoundedRect $g $cardBrush $x $y $cardW $cardH 18
        $borderPen = New-Object System.Drawing.Pen (New-Color "#bfdbfe"), 2
        $g.DrawRectangle($borderPen, $x + 2, $y + 2, $cardW - 4, $cardH - 4)
        $g.FillEllipse($accentBrush, $x + 16, $y + 24, 48, 48)
        $num = "{0:00}" -f ($i + 1)
        $numSize = $g.MeasureString($num, $numFont)
        $g.DrawString($num, $numFont, $white, $x + 40 - $numSize.Width / 2, $y + 38)
        Draw-WrappedText $g ([string]$diagram.steps[$i]) $cardFont $textBrush ($x + 78) ($y + 28) 210 28

        if ($i -lt $diagram.steps.Count - 1) {
            $nextRow = [Math]::Floor(($i + 1) / $cols)
            if ($nextRow -eq $row) {
                $x1 = $x + $cardW + 10
                $y1 = $y + $cardH / 2
                $x2 = $x + $cardW + $gapX - 16
                $g.DrawLine($mutedPen, $x1, $y1, $x2, $y1)
                $arrowBrush = New-Object System.Drawing.SolidBrush (New-Color "#64748b")
                $ax1 = [float]$x2
                $ay1 = [float]$y1
                $ax2 = [float]($x2 - 12)
                $ay2 = [float]($y1 - 8)
                $ax3 = [float]($x2 - 12)
                $ay3 = [float]($y1 + 8)
                $g.FillPolygon($arrowBrush, @(
                    (New-Object System.Drawing.PointF($ax1, $ay1)),
                    (New-Object System.Drawing.PointF($ax2, $ay2)),
                    (New-Object System.Drawing.PointF($ax3, $ay3))
                ))
            }
        }
    }

    $footBrush = New-Object System.Drawing.SolidBrush (New-Color "#0f172a")
    Draw-RoundedRect $g $footBrush 85 870 1430 60 18
    $footFont = New-Object System.Drawing.Font("Microsoft YaHei", 17, [System.Drawing.FontStyle]::Bold)
    $g.DrawString($footer, $footFont, $white, 110, 887)

    $path = Join-Path $OutDir ([string]$diagram.file)
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose()
    $bmp.Dispose()
}

$json = Get-Content -Raw -Encoding UTF8 -Path $ConfigPath | ConvertFrom-Json
foreach ($diagram in $json.diagrams) {
    New-FlowPng $diagram ([string]$json.footer)
}

Write-Host "Generated $($json.diagrams.Count) PNG flow diagrams in $OutDir"
