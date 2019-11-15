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

recycling_labels = [
    'plastics',
    'bottles',
    'paper',
    'cardboard',
    'cereal',
    'boxes',
    'magazines',
    'mail',
    'office paper',
    'newspaper',
    'metal',
    'tin',
    'aluminum',
    'steel',
    'can',
    'glass',
    'soft drink',
    'beer',
]

trash_labels = [
    'plastic bags',
    'plastic stretch wrap',
    'foam cups',
    'foam',
    'plastic utensils',
    'grocery bags',
    'bottle caps',
    'styrofoam',
    'wrapper',
    'chip bags',
    'chips',
    'potato chip',
    'food storage containers'
]

compost_labels = [
    'meat',
    'fish',
    'dairy',
    'fruit',
    'vegetables',
    'shells',
    'bones',
    'pasta',
    'rice',
    'eggshells',
    'nutshells',
    'bread',
    'grains',
    'scraps',
    'leftovers',
    'coffee',
    'coffee grounds',
    'coffee filters',
    'tea bags',
    'soiled',
    'soiled paper bags',
    'paper towels',
    'paper napkins',
    'paper egg cartons',
    'paper plates',
    'plants',
    'flowers',
    'vegetation',
    'wood',
    'scraps'
]

def get_fine_grain_labels(google_cloud_labels):
    fine_grain_labels = []
    for elem in google_cloud_labels:
        fine_grain_labels.extend(elem.split(' '))
    fine_grain_labels.extend(b)
    return fine_grain_labels

def get_classification(fine_grain_labels):
    return None



# location = '/Users/btl787/Google Drive/00-fa19/03-eecs-249a-embedded-systems/project/crumpled_napkin.jpg'
#location = '/home/pi/Desktop/crumpled_napkin.jpg'

google_cloud_labels = get_labels(location)
fine_grain_labels = get_fine_grain_labels(google_cloud_labels)
print(google_cloud_labels)