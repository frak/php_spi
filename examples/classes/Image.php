<?php
/**
 * A class to convert images into a re-sized bit array for sending to LEDs
 *
 * @author Michael Davey
 */
class Image
{
    /**
     * Lazy loaded array of gamma corrections for colours
     */
    static private $gammas;

    /**
     * The image pixel array
     */
    private $data;

    /**
     * Constructor
     *
     * @param string $src The path to the source image file on disk
     * @param int $height Optional height of the images, defaults to 32
     * @throws InvalidArgumentException If the image does not exist of have a valid type
     */
    public function __construct($src, $height = 32)
    {
        // Sanity check
        if(!file_exists($src)) {
            throw new InvalidArgumentException("File {$src} does not exist");
        }

        $image = NULL;
        $image = $this->load($src);


        // Resize the image
        $newImage = $this->resize($image, $height);

        $newWidth = imagesx($newImage);
        // Load into the data array, having applied gamma correction
        for($i = 0; $i < $newWidth; ++$i) {
            for($j = 0; $j < $height; ++$j) {
                $rgb = imagecolorat($newImage, $i, $j);
                $rgb = imagecolorsforindex($newImage, $rgb);
                $j3  = $j * 3;
                $this->data[$i][$j3]     = $this->gamma($rgb['green']);
                $this->data[$i][$j3 + 1] = $this->gamma($rgb['red']);
                $this->data[$i][$j3 + 2] = $this->gamma($rgb['blue']);
            }
        }
    }

    /**
     * Load image file according to type
     *
     * @param string $src The path to the file on disk
     * @return resource The GD image resource
     * @throws InvalidArgumentException If the file extension is not supported
     */
    private function load($src)
    {
        $parts = explode('.', $src);
        $ext = $parts[count($parts) - 1];
        switch ($ext) {
            case 'png':
                $image = imagecreatefrompng($src);
                return $image;
            case 'jpeg':
            case 'jpg':
                $image = imagecreatefromjpeg($src);
                return $image;
            case 'gif':
                $image = imagecreatefromgif($src);
                return $image;
            default:
                throw new InvalidArgumentException("Unknown image format: {$ext}");
        }
    }

    /**
     * Return the specified column of data from the image
     * data array
     *
     * @param int $column The column of data to return
     * @return array The column of bytes
     */
    public function get($column)
    {
        return $this->data[$column];
    }

    /**
     * Returns the entire image array as an array of arrays
     *
     * @return array The image data
     */
    public function getAll()
    {
        return $this->data;
    }

    /**
     * Returns the resultant resized image
     *
     * @param $image resource The original GD image resource
     * @param int $newHeight The height of the new image
     * @return resource The resultant GD image resource
     */
    private function resize($image, $newHeight)
    {
        $width    = imagesx($image);
        $height   = imagesy($image);
        $ratio    = $height / $newHeight;
        $newWidth = $width / $ratio;
        $newImage = imagecreatetruecolor($newWidth, $newHeight);
        imagecopyresized(
            $newImage,
            $image,
            0, 0, 0, 0,
            $newWidth, $newHeight,
            $width, $height
        );
        return $newImage;
    }

    /**
     * Helper function to calculate and store the gamma corrections
     * for colour values
     *
     * @param int $level The level of colour intensity
     * @return int The corrected colour value
     */
    private function gamma($level)
    {
        if(!isset(self::$gammas[$level])) {
            $gamma = 0x80 | (int) (pow($level / 255, 2.5) * 127 + 0.5);
            self::$gammas[$level] = $gamma;
        }
        return self::$gammas[$level];
    }

}