from django.db import models
from user.models import Customer

# Create your models here.

ORDER_STATUS = {
  ("open", "open"), # things in the shopping cart, can still add more item to it, can only have one for each customer
  ("closed", "closed"), # order confirmed, alredy checked out, can not add anything more
  ("one_time", "one_time"), # orders related to buy now, can have multiple of it, will change to closed when checked out
}

ITEM_STATUS = {
  ('new', 'new'),
  ('packed', 'packed'),
  ('delivering', 'delivering'),
  ('delivered', 'delivered'),
}

class Category(models.Model):
  c_id = models.AutoField(primary_key=True)
  name = models.CharField(max_length=100, unique=True)

class Product(models.Model):
  p_id = models.AutoField(primary_key=True)
  category = models.ForeignKey(Category, on_delete = models.CASCADE) #should never delete category
  name = models.CharField(max_length=100, unique=True)
  price = models.DecimalField(max_digits=10, decimal_places=2)

class Warehouse(models.Model):
  w_id = models.AutoField(primary_key=True)
  loc_x = models.IntegerField()
  loc_y = models.IntegerField()

class Inventory(models.Model):
  product = models.ForeignKey(Product, on_delete = models.CASCADE)
  warehouse = models.ForeignKey(Warehouse, on_delete = models.CASCADE)
  count = models.PositiveIntegerField()

  class Meta:
    unique_together = (("product", "warehouse"),)

class Order(models.Model):
  o_id = models.AutoField(primary_key=True)
  customer = models.ForeignKey(Customer, on_delete = models.CASCADE)
  loc_x = models.IntegerField(blank=True, null=True)
  loc_y = models.IntegerField(blank=True, null=True)
  ups_account = models.TextField(default="", blank=True)
  card_number = models.DecimalField(max_digits=16, decimal_places=0, blank=True, null=True)
  status = models.CharField(default='open', choices=ORDER_STATUS, max_length=10)
  date_time = models.DateTimeField(auto_now=True)

class Item(models.Model):
  i_id = models.AutoField(primary_key=True)
  order = models.ForeignKey(Order, on_delete = models.CASCADE, related_name='items')
  product = models.ForeignKey(Product, on_delete = models.CASCADE) # We should never delete products, could set inventory to 0 instead
  count = models.PositiveIntegerField()
  status = models.CharField(default='new', choices=ITEM_STATUS, max_length=12)
  ups_truckid = models.IntegerField(null=True, blank=True)
