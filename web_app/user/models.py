from django.db import models
from django.contrib.auth.models import User

# Create your models here.

class Customer(models.Model):
  user = models.OneToOneField(User, on_delete = models.CASCADE)
  loc_x = models.IntegerField(blank=True, null=True)
  loc_y = models.IntegerField(blank=True, null=True)
  ups_account = models.TextField(blank=True, default='');
  card_number = models.DecimalField(max_digits=16, decimal_places=0, blank=True, null=True)
