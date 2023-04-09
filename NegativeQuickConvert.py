import os
import cv2
import time
import argparse
import numpy as np
from multiprocessing import Pool

class QuickNegativeProcessor:
    def __init__(self, conv_type, quality=50) -> None:
        self.conv_type = conv_type
        self.jpeg_quality = quality
        self.read_path = None
        self.save_path = None

    def read_Img(self, name):
        img = cv2.imread(os.path.join(self.read_path, name))
        bgr_img = cv2.split(img) # Split image into b,g,r channels
        return bgr_img

    def read_bw_Img(self, name):
        bw_img = cv2.imread(os.path.join(self.read_path, name), cv2.IMREAD_GRAYSCALE)
        return bw_img

    def stretch(self, plane):
        imin = np.percentile(plane, 1)
        imax = np.percentile(plane, 99)

        # stretch image
        plane = (plane - imin) / (imax - imin)

        return plane

    def conv_bgr_Img(self, neg):
        b = 1 - self.stretch(neg[0])
        g = 1 - self.stretch(neg[1])
        r = 1 - self.stretch(neg[2])

        b *= 0.5 / b.mean()
        g *= 0.5 / g.mean()
        r *= 0.5 / r.mean()

        img = cv2.merge([b,g,r])
        img *= 255
        return img

    def conv_bw_Img(self,neg):
        bw = 1 - self.stretch(neg)

        bw *= 0.5 / bw.mean()
        bw *= 255

        return bw


    def writeImg(self, name, img):
        cv2.imwrite(os.path.join(self.save_path, name), img, [cv2.IMWRITE_JPEG_QUALITY, self.jpeg_quality])


    def run_batch(self, name_list):
        for img_name in name_list:
            name = img_name
            img = None
            if self.conv_type == "bw":
                img = self.read_bw_Img(name)
                img = self.conv_bw_Img(img)
            elif self.conv_type == "color":
                img = self.read_Img(name)
                img = self.conv_bgr_Img(img)
            else:
                raise Exception("conv_type was not color or bw!")
            if img is None:
                raise Exception("Image was not processed for some reason!")
            
            self.writeImg(name, img)

def getSublists(lst,n):
    subListLength = len(lst) // n 
    return [lst[i:i + subListLength] for i in range(0, len(lst), subListLength)]


def main(conv_type="bw", multicore=False, quality=50):
    path = args.path[0]
    conv_type = args.negative_type[0]
    multicore = args.multicore
    quality = args.quality

    # Get list of all files in given directory
    all_files_list = os.listdir(path)
    print(all_files_list)
    img_lists = []
    # Reject any file/directory that is not a jpg
    acceptable_formats = [".jpg", ".jpeg"]
    for img_name in all_files_list:
        if img_name[-4:].lower() not in acceptable_formats:
            continue
        
        img_lists.append(img_name)

    qnp = QuickNegativeProcessor(conv_type, quality)
    qnp.read_path = path
    qnp.save_path = os.path.join(path, "QuickNegativeConvert")
    if not os.path.exists(qnp.save_path):
        os.makedirs(qnp.save_path)

    if multicore:
        core_count = os.cpu_count()
        # Core counts larger than number of images causes problems
        if core_count > len(img_lists):
            core_count = len(img_lists)

        # Split list of names for each processor core
        names = getSublists(img_lists, core_count)

        p = Pool(core_count)
        p.map(qnp.run_batch, names)
    else:
        # Process one image at a time
        qnp.run_batch(img_lists)


if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(
        description="""Quickly convert color and BW negatives to positive images.""",
        formatter_class=argparse.RawTextHelpFormatter,
    )
    parser.add_argument("path", nargs="+", help="Path to folder")
    parser.add_argument("negative_type", nargs="+", help="color or bw")
    parser.add_argument('-t', '--multicore', action='store_true', help="Attempt to use all processor cores (faster)")
    parser.add_argument('-q', '--quality', 
        default='50',
        dest='quality', 
        action='store', 
        type=int,
        help="Set the jpg compression quality (default: 50)")
    args = parser.parse_args()
    start_time = time.time()

    main(args)
    print("All images processed in %g seconds ---" % (time.time() - start_time))