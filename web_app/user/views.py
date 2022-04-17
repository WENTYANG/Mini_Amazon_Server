from .models import Customer
from django.urls import reverse
from django.contrib import messages
from django.contrib.auth.models import User
from django.shortcuts import render
from django.http import HttpResponseRedirect, HttpResponse
from django.contrib.auth.decorators import login_required
from .forms import UserProfileCreateForm, UserProfileChangeForm, OptionalProfileForm

# Create your views here.

def welcome(request):
  return render(request, 'user/welcome.html')

#def logout(request):
#  return render(request, 'user/logout.html')

def create_account(request):
  if request.method == "GET":
    user_form = UserProfileCreateForm()
    optional_form = OptionalProfileForm()
    return render(request, 'user/create_account.html', {'user_form': user_form, 'optional_form': optional_form})
  else:
    user_form = UserProfileCreateForm(request.POST)
    optional_form = OptionalProfileForm(request.POST)
    if user_form.is_valid() and optional_form.is_valid():
      #if User.objects.filter(username=user_form.cleaned_data.get('username')).exists() == False:
        user = user_form.save()
        customer = Customer(user=user, loc_x=optional_form.cleaned_data.get('loc_x'), \
            loc_y=optional_form.cleaned_data.get('loc_y'), ups_account=optional_form.cleaned_data.get('ups_account'),
            card_number=optional_form.cleaned_data.get('card_number'))
        customer.save()
        messages.add_message(request, messages.INFO, 'Create Successfully! Please log in.')
        return HttpResponseRedirect(reverse('user:login'))
      #else:
       # return render(request, 'user/create_account.html', {'existed_user': True})
    elif optional_form.is_valid()==False:
      #user_form = UserProfileCreateForm()
      #optional_form = OptionalProfileForm()
      return render(request, 'user/create_account.html', {'user_form': user_form, 'optional_form': optional_form, 'invalid_user': True})
    else:
      return render(request, 'user/create_account.html', {'user_form': user_form, 'optional_form': optional_form})
        
@login_required
def change_info(request):
  if request.method == "GET":
    user_form = UserProfileChangeForm(instance=request.user)
    optional_form = OptionalProfileForm(instance=request.user.customer)
    return render(request, 'user/change_info.html', {'user_form': user_form, 'optional_form': optional_form})
  else:
    user_form = UserProfileChangeForm(request.POST, instance=request.user)
    optional_form = OptionalProfileForm(request.POST, instance=request.user.customer)
    if user_form.is_valid() and optional_form.is_valid():
      user_form.save()
      optional_form.save()
      messages.add_message(request, messages.INFO, 'Edit User Profile Successfully!')
      return HttpResponseRedirect(reverse('user:show_info'))
    else:
      #messages.add_message(request, messages.INFO, 'Something went wrong when editing user profile. Please try again!')
      return render(request, 'user/change_info.html', {'error_msg': 'Have invalid info when editing user profile. Please try again.', 'user_form': user_form, 'optional_form': optional_form})

@login_required
def show_info(request):
    return render(request, 'user/show_info.html')
