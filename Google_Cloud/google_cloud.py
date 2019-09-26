from google.cloud import vision
import io

def get_labels(path):
    client = vision.ImageAnnotatorClient()
    with io.open(path, 'rb') as image_file:
        content = image_file.read()

    image = vision.types.Image(content=content)

    response = client.label_detection(image=image)
    labels = response.label_annotations
    return [label.description for label in labels]

location = '/Users/btl787/Google Drive/00-fa19/03-eecs-249a-embedded-systems/project/crumpled_napkin.jpg'
# location = '/home/pi/Desktop/crumpled_napkin.jpg'
labels = get_labels(location)
print(labels)