from django.shortcuts import render
from django.contrib.auth.decorators import login_required
from amazon.models import Category, Product, Order, Item

# Create your views here.
@login_required
def home(request):
  products=Product.objects.all()
  return render(request, 'amazon/home.html', {'products': products});

@login_required
def search_by_cat(request):
  return render(request, 'amazon/search_by_cat.html');

@login_required
def goto_cart(request):
  return render(request, 'amazon/goto_cart.html');
   
@login_required
def view_order(request):
  return render(request, 'amazon/view_order.html');

@login_required
def view_product(request, p_id):
  product=Product.objects.get(p_id=p_id)
  return render(request, 'amazon/view_product.html', {'product': product});




