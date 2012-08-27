<?php

class SpiImage
{
    const HEIGHT = 32;

    static private $gammas;

    private $data;

    public function __construct($src)
    {
        // Sanity check
        if(!file_exists($src)) {
            throw new InvalidArgumentException("File {$src} does not exist");
        }

        // Load image file according to type
        $parts = explode('.', $src);
        $ext   = $parts[count($parts) - 1];
        $image = 0;
        switch($ext) {
            case 'png':
                $image = imagecreatefrompng($src);
                break;
            case 'jpeg':
            case 'jpg':
                $image = imagecreatefromjpeg($src);
                break;
            case 'gif':
                $image = imagecreatefromgif($src);
                break;
            default:
                throw new InvalidArgumentException("Unknown image format: {$ext}");
                break;
        }

        // Get image dimensions
        list($width, $height) = getimagesize($src);
        list($newWidth, $newHeight) = $this->resize($width, $height);

        // Resize the image
        $newImage = imagecreatetruecolor($newWidth, $newHeight);
        imagecopyresized($newImage, $image, 0, 0, 0, 0, $newWidth, $newHeight, $width, $height);

        // Load into the data array, having applied gamma correction
        for($i = 0; $i < $newWidth; ++$i) {
            for($j = 0; $j < self::HEIGHT; ++$j) {
                $rgb = imagecolorat($newImage, $i, $j);
                $rgb = imagecolorsforindex($newImage, $rgb);
                $j3  = $j * 3;
                $this->data[$i][$j3]     = $this->gamma($rgb['green']);
                $this->data[$i][$j3 + 1] = $this->gamma($rgb['red']);
                $this->data[$i][$j3 + 2] = $this->gamma($rgb['blue']);
            }
        }
    }

    public function get($column)
    {
        return $this->data[$column];
    }

    private function resize($width, $height)
    {
        $ratio = $height / self::HEIGHT;
        return array(
            $width / $ratio,
            self::HEIGHT
        );
    }

    private function gamma($level)
    {
        if(!isset(self::$gammas[$level])) {
            $gamma = 0x80 | (int) (pow($level / 255, 2.5) * 127 + 0.5);
            self::$gammas[$level] = $gamma;
        }
        return $gammas[$level];
    }

}