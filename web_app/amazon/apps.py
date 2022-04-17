from django.apps import AppConfig
from django.db.models.signals import post_migrate

category = ['fruits', 'snacks', 'drinks'] 
product = ['apple', 'banana', 'orange', 'chocolate', 'cookie', 'water']
cat_belonged = [0, 0, 0, 1, 1, 2]
price = [2.99, 1.99, 2.49, 2.29, 5.49, 0.99]

warehouse_locx = [0, 100]
warehouse_locy = [0, 100]

def add_warehouse():
  from amazon.models import Warehouse
  if Warehouse.objects.all().count() == 0: # no warehouse
    for i in range(len(warehouse_locx)):
      Warehouse.objects.create(loc_x=warehouse_locx[i], loc_y=warehouse_locy[i])

def add_product():
  from amazon.models import Warehouse, Category, Product, Inventory
  if Product.objects.all().count() == 0: # no product
    for i in range(len(category)):
      if Category.objects.filter(name=category[i]).exists() == False:
        Category.objects.create(name=category[i])
    for i in range(len(product)):
      #print(i)
      c = Category.objects.get(name=category[cat_belonged[i]]);
      p = Product.objects.create(category=c, name=product[i], price=price[i])
      for w in Warehouse.objects.all():
        Inventory.objects.create(product=p, warehouse=w, count=0)

def amazon_callback(sender, **kwargs):
  add_warehouse()
  add_product()

class AmazonConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'amazon'

    def ready(self):
      post_migrate.connect(amazon_callback, sender=self)


