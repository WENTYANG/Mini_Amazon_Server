from django.forms import ModelForm
from django import forms
from .models import Order, Item

class OrderInfoForm(ModelForm):
  loc_x = forms.IntegerField()
  loc_y = forms.IntegerField()
  card_number = forms.DecimalField(max_digits=16, decimal_places=0)
  
  class Meta:
    model = Order
    fields = ['loc_x', 'loc_y', 'ups_account', 'card_number']

class ItemAmountForm(ModelForm):
  count = forms.IntegerField(min_value=1, max_value=100)
  class Meta:
    model = Item
    fields = ['count']
