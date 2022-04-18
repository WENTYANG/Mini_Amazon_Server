from django.forms import ModelForm
from django import forms
from .models import Customer
from django.contrib.auth.models import User
from django.contrib.auth.forms import UserCreationForm

class UserProfileCreateForm(UserCreationForm):
  email = forms.EmailField()
  class Meta:
    model = User
    fields = ['username', 'email', 'password1', 'password2']

class UserProfileChangeForm(ModelForm):
  email = forms.EmailField()
  class Meta:
    model = User
    fields = ['email']

class OptionalProfileForm(ModelForm):
    class Meta:
        model = Customer 
        fields = ['loc_x', 'loc_y', 'ups_account', 'card_number']

